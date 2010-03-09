var windows=[];

function win(cl) {
  this.win=document.createElement("div");
  this.win.className=cl;
  this.content=document.createElement("div");
  this.id=uniqid();
  windows[this.id]=this;

  document.body.appendChild(this.win);
  this.win.appendChild(this.content);

  this.close=function() {
    this.win.parentNode.removeChild(this.win);
    delete windows[this.id];
  }
}

function window_close(id) {
  windows[id].close();
}
