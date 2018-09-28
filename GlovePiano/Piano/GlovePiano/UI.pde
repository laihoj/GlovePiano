////------------------------------------------
////UI Functionality
////------------------------------------------



void mousePressed()
{
  //keyboard button -- toggle virtual keyboard
  if(mouseY <= 100 && mouseX > 0 && mouseX < width/3) 
    KetaiKeyboard.toggle(this);
  //configuration button -- go to configuration view
  //else if(mouseY <= 100 && mouseX > width/3 && mouseX < 2*(width/3)) {
  //  isConfiguring=true;
  //}
}

public void keyPressed() {
  if(key =='c')
  {
    //If we have not discovered any devices, try prior paired devices
    if(bt.getDiscoveredDeviceNames().size() > 0)
      klist = new KetaiList(this, bt.getDiscoveredDeviceNames());
    else if(bt.getPairedDeviceNames().size() > 0)
      klist = new KetaiList(this, bt.getPairedDeviceNames());
  }
  else if(key == 'd') {
    bt.discoverDevices();
  } else if(key == 'x')
    bt.stop();
  else if(key == 'b') {
    bt.makeDiscoverable();
  }
  else if(key == 's') {
    bt.start();
  }
}



  //------------------------------------------
  //Interaction screen with up and down arrows
  //------------------------------------------
void drawPiano() {
}
  //------------------------------------------
  //Configuration screen with command explanations
  //------------------------------------------

void drawConfigurationScreen() {
  
  UIText =  "d - discover devices\n" +
    "b - make this device discoverable\n" +
    "c - connect to device from discovered list.\n" +
    "p - list paired devices\n" +
    "i - Bluetooth info\n" +
    "a - Acceleration info";
  ArrayList<String> names;
  background(BACKGROUND_COLOR);

  //based on last key pressed lets display
  if(key == 'i') info = getBluetoothInformation(); else 
  if(key =='a') info = getAccelInformation(); else 
  {
    if(key == 'p') {
      info = "Paired Devices:\n";
      names = bt.getPairedDeviceNames();
    } else {
      info = "Discovered Devices:\n";
      names = bt.getDiscoveredDeviceNames();
    }

    for(int i=0; i < names.size(); i++) {
      info += "["+i+"] "+names.get(i).toString() + "\n";
    }
  }
  fill(0);
  text(UIText + "\n\n" + info, 10, 140);
}

void drawUI() {
  //Draw top shelf UI buttons

  pushStyle();
  
  //Keyboard button
  fill(0);
  stroke(255);
  rect(0, 0, width/3, 100);
  fill(255);
  text("Keyboard", 5, 60);
  
  
  //Configuring button
  if(CONFIGURING) {
    noStroke();
    fill(255, 128, 0);
  } else fill(0);
  rect(width/3, 0, width/3, 100);
  fill(255);
  text("Configuration", width/3+5, 60);
  
  
  if(!CONFIGURING) {  
    noStroke();
    fill(255, 128, 0);
  }
  else {
    fill(0);
    stroke(255);
  }  
  rect((width/3)*2, 0, width/3, 100);
  fill(255);
  text("Interact", width/3*2+5, 60); 
  
  popStyle();
}

void onKetaiListSelection(KetaiList klist)
{
  String selection = klist.getSelection();
  bt.connectToDeviceByName(selection);

  //dispose of list for now
  klist = null;
}