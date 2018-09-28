private void prepareExitHandler() {
  Runtime.getRuntime().addShutdownHook(new Thread(new Runnable() {
    public void run () {
      System.out.println("SHUTDOWN HOOK");
      bt.broadcast(("&0,0,0,0,!").getBytes());
      bt.broadcast(("@PHONE APP CLOSED").getBytes());
    }
  }));
}