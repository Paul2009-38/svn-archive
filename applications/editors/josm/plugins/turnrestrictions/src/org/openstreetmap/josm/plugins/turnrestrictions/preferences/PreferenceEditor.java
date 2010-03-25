package org.openstreetmap.josm.plugins.turnrestrictions.preferences;

import static org.openstreetmap.josm.tools.I18n.tr;

import java.awt.BorderLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;

import org.openstreetmap.josm.Main;
import org.openstreetmap.josm.gui.preferences.PreferenceSetting;
import org.openstreetmap.josm.gui.preferences.PreferenceTabbedPane;
import org.openstreetmap.josm.gui.widgets.HtmlPanel;
import org.openstreetmap.josm.tools.GBC;
import org.openstreetmap.josm.tools.ImageProvider;
import org.openstreetmap.josm.tools.OpenBrowser;

/**
 * This is the preference editor for the turn restrictions plugin.
 *
 */
public class PreferenceEditor extends JPanel implements PreferenceSetting{
	
	private IconPreferencePanel pnlIconPreferences;

	/**
	 * builds the panel with the sponsoring information 
	 * 
	 * @return
	 */
	protected JPanel buildCreditPanel() {
		JPanel pnl = new JPanel(new GridBagLayout());
		pnl.setBorder(BorderFactory.createEmptyBorder(5,5,5,5));
		GridBagConstraints gc = new GridBagConstraints();
		gc.anchor = GridBagConstraints.NORTHWEST;
		gc.fill = GridBagConstraints.HORIZONTAL;
		gc.insets = new Insets(0, 0,0, 5);
		gc.weightx = 0.0;
		JLabel lbl = new JLabel();
		pnl.add(lbl, gc);
		lbl.setIcon(ImageProvider.get("skobbler-logo"));
		
		gc.gridx = 1;
		gc.weightx = 1.0;
		HtmlPanel msg  =new HtmlPanel();
		msg.setText("<html><body>"
				+ tr("Development of the turn restriction plugin was sponsored " 
				+ "by <a href=\"http://www.skobbler.de\">skobbler GmbH</a>.")
				+"</body></html>");
		pnl.add(msg, gc);
		
		// filler - grab remaining space 
		gc.gridy = 1;
		gc.gridx = 0;
		gc.gridwidth = 2;
		gc.weightx = 1.0;
		gc.weighty = 1.0;
		pnl.add(new JPanel(), gc);
		
		SkobblerUrlLauncher urlLauncher = new SkobblerUrlLauncher();
		msg.getEditorPane().addHyperlinkListener(urlLauncher);
		lbl.addMouseListener(urlLauncher);
		return pnl;
	}

	protected JPanel buildIconPreferencePanel() {
		JPanel pnl = new JPanel(new BorderLayout());
		
		pnlIconPreferences = new IconPreferencePanel();
		pnlIconPreferences.initFromPreferences(Main.pref);
		
		JScrollPane sp = new JScrollPane(pnlIconPreferences);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
		
		pnl.add(sp, BorderLayout.CENTER);
		return pnl;
	}
	
	protected void build() {
		setLayout(new BorderLayout());
		JTabbedPane tp = new JTabbedPane();
		tp.add(buildIconPreferencePanel());
		tp.add(buildCreditPanel());		
		tp.setTitleAt(0, tr("Road Signs"));
		tp.setToolTipTextAt(0,tr("Configure the set of road sign icons to be used in the turnrestrictions plugin"));
		tp.setTitleAt(1, tr("Sponsor"));
		add(tp, BorderLayout.CENTER);
	}
	
	public PreferenceEditor() {
		build();
	}
	
	public void addGui(PreferenceTabbedPane gui) {
		String description = tr("An OSM plugin for editing turn restrictions.");
		JPanel tab = gui.createPreferenceTab("turnrestrictions", tr("Turn Restrictions"), description);
        tab.add(this, GBC.eol().fill(GBC.BOTH));
	}

	public boolean ok() {
		pnlIconPreferences.saveToPreferences(Main.pref);
		return false;
	}
	
	/**
	 * Launches an external browser with the sponsors home page 
	 */
	class SkobblerUrlLauncher extends MouseAdapter implements HyperlinkListener {
		protected void launchBrowser() {
			OpenBrowser.displayUrl("http://www.skobbler.de");
		}
		
		public void hyperlinkUpdate(HyperlinkEvent e) {
			if (e.getEventType().equals(HyperlinkEvent.EventType.ACTIVATED)) {
				launchBrowser();
			}
		}

		@Override
		public void mouseClicked(MouseEvent e) {
			launchBrowser();
		}
	}
}
