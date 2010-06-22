package ext_tools;

import static org.openstreetmap.josm.tools.I18n.tr;

import java.awt.Cursor;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.StringTokenizer;

import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;

import org.openstreetmap.josm.Main;
import org.openstreetmap.josm.command.Command;
import org.openstreetmap.josm.command.SequenceCommand;
import org.openstreetmap.josm.data.ProjectionBounds;
import org.openstreetmap.josm.data.coor.LatLon;
import org.openstreetmap.josm.data.osm.DataSet;
import org.openstreetmap.josm.gui.MainMenu;
import org.openstreetmap.josm.gui.PleaseWaitRunnable;
import org.openstreetmap.josm.gui.progress.NullProgressMonitor;
import org.openstreetmap.josm.io.IllegalDataException;
import org.openstreetmap.josm.io.OsmReader;

public class ExtTool {

    protected boolean enabled;
    public String name;
    public String cmdline;
    public String description;
    public String url;
    protected ExtToolAction action;
    protected JMenuItem menuItem;

    public boolean isEnabled() {
        return enabled;
    }

    public void setEnabled(boolean enabled) {
        if (!this.enabled ^ enabled)
            return;
        this.enabled = enabled;
        if (enabled) {
            if (action == null)
                action = new ExtToolAction(this);
            menuItem = MainMenu.add(Main.main.menu.toolsMenu, action);
        } else {
            Main.main.menu.toolsMenu.remove(menuItem);
        }
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public ExtTool() {
        this.enabled = false;
    }

    public ExtTool(String name) {
        this();
        this.name = name;
    }

    public String serialize() {
        StringBuilder sb = new StringBuilder();
        sb.append("name=").append(name).append('\n');
        sb.append("cmdline=").append(cmdline).append('\n');
        sb.append("description=").append(description).append('\n');
        sb.append("url=").append(url).append('\n');
        sb.append('\n');
        return sb.toString();
    }

    public static ExtTool unserialize(String str) {
        ExtTool t = new ExtTool();
        String[] lines = str.split("\n");
        for (String line : lines) {
            String[] parts = line.split("=", 2);
            if (parts[0].equals("name"))
                t.name = parts[1];
            else if (parts[0].equals("cmdline"))
                t.cmdline = parts[1];
            else if (parts[0].equals("description"))
                t.description = parts[1];
            else if (parts[0].equals("url"))
                t.url = parts[1];
        }
        return t;
    }

    private class ToolProcess {
        public Process process;
        public volatile boolean running;
    }

    static double getPPD() {
        ProjectionBounds bounds = Main.map.mapView.getProjectionBounds();
        return Main.map.mapView.getWidth() /
                (bounds.max.east() - bounds.min.east());
    }

    public void runTool(LatLon pos) {
        Main.map.mapView.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
        // parse cmdline and build cmdParams array
        HashMap<String, String> replace = new HashMap<String, String>();

        replace.put("{lat}", "" + pos.lat());
        replace.put("{lon}", "" + pos.lon());
        replace.put("{PPD}", "" + getPPD());

        ArrayList<String> cmdParams = new ArrayList<String>();
        StringTokenizer st = new StringTokenizer(cmdline);

        while (st.hasMoreTokens()) {
            String token = st.nextToken();
            if (replace.containsKey(token)) {
                cmdParams.add(replace.get(token));
            } else {
                cmdParams.add(token);
            }
        }

        // create the process
        final Object syncObj = new Object();

        ProcessBuilder builder;
        builder = new ProcessBuilder(cmdParams);
        builder.directory(new File(ExtToolsPlugin.plugin.getPluginDir()));

        // debug: print resulting cmdline
        for (String s : builder.command())
            System.out.print(s + " ");
        System.out.print("\n");

        final ToolProcess tp = new ToolProcess();
        try {
            tp.process = builder.start();
        } catch (IOException e1) {
            return;
        }
        tp.running = true;

        // redirect child process's stderr to JOSM stderr
        new Thread(new Runnable() {
            public void run() {
                try {
                    byte[] buffer = new byte[1024];
                    InputStream errStream = tp.process.getErrorStream();
                    int len;
                    while ((len = errStream.read(buffer)) > 0) {
                        System.err.write(buffer, 0, len);
                    }
                } catch (IOException e) {
                }
            }
        }).start();

        // read stdout stream
        Thread osmParseThread = new Thread(new Runnable() {
            public void run() {
                try {
                    final InputStream inputStream = tp.process.getInputStream();
                    final DataSet ds = OsmReader.parseDataSet(inputStream,
                            NullProgressMonitor.INSTANCE);
                    final List<Command> cmdlist = new DataSetToCmd(ds).getCommandList();
                    if (!cmdlist.isEmpty()) {
                        SequenceCommand cmd =
                                new SequenceCommand(getName(), cmdlist);
                        Main.main.undoRedo.add(cmd);
                    }
                } catch (IllegalDataException e) {
                    e.printStackTrace();
                    if (tp.running) {
                        tp.process.destroy();
                        SwingUtilities.invokeLater(new Runnable() {
                            public void run() {
                                JOptionPane.showMessageDialog(Main.parent,
                                        tr("Child script have returned invalid data."));
                            }
                        });
                    }
                } finally {
                    synchronized (syncObj) {
                        tp.running = false;
                        syncObj.notifyAll();
                    }
                }
            }

        });
        osmParseThread.start();

        synchronized (syncObj) {
            try {
                syncObj.wait(10000);
            } catch (InterruptedException e) {
            }
        }
        if (tp.running) {
            new Thread(new PleaseWaitRunnable(name) {
                @Override
                protected void realRun() {
                    try {
                        progressMonitor.indeterminateSubTask(null);
                        synchronized (syncObj) {
                            if (tp.running)
                                syncObj.wait();
                        }
                    } catch (InterruptedException e) {
                    }
                }

                @Override
                protected void cancel() {
                    synchronized (syncObj) {
                        tp.running = false;
                        tp.process.destroy();
                        syncObj.notifyAll();
                    }
                }

                @Override
                protected void finish() {
                }
            }).start();
        }
    }
}
