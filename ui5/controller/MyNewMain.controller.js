sap.ui.define(['rootui5/eve7/controller/Main.controller', 'rootui5/eve7/lib/EveManager'],
function(MainController, EveManager) {
  "use strict";

  return MainController.extend("custom.MyNewMain", {

    /// On websocket opened
    OnWebsocketOpened : function() {
      // Set the connexion status to 'green'
      var element = this.byId("connexion-status");
      element.setHtmlText("Connected");
      element.toggleStyleClass("disconnected-status", false ) ;
      element.toggleStyleClass("connected-status", true ) ;
    },

    /// On websocket closed
    OnWebsocketClosed : function() {
      // Set the connexion status to 'red'
      var element = this.byId("connexion-status");
      element.setHtmlText("Disconnected");
      element.toggleStyleClass("disconnected-status", true ) ;
      element.toggleStyleClass("connected-status", false ) ;
      // Disable navigation on disconnect
      this.byId('prevEvent').setEnabled( false ) ;
      this.byId('nextEvent').setEnabled( false ) ;
    },

    /// Open controller init (openui)
    onInit: function() {
      MainController.prototype.onInit.apply(this, arguments);
      this.mgr.handle.SetReceiver(this);
      console.log("register my controller for init");
      this.mgr.RegisterController(this);
      // Set the connexion status to 'green'
      var element = this.byId("connexion-status");
      element.setHtmlText("Disconnected");
      element.toggleStyleClass("disconnected-status", true ) ;
      element.toggleStyleClass("connected-status", false ) ;
    },

    /// On EVE manager init
    OnEveManagerInit: function() {
      MainController.prototype.OnEveManagerInit.apply(this, arguments);
      // TODO: find a better accessor here...
      this.world = this.mgr.childs[0].childs;
      // Find the event navigator and event display instances in EVE
      function findElement( name, element ) {
        console.log( "Is it ", element, " ?" )
        if( ! element.hasOwnProperty('_typename') ) {
          return false;
        }
        return (element._typename == name ) ;
      }
      this.eventMgr = this.world.find(findElement.bind(null, "lceve::EventNavigator")) ;
      this.eventDisplay = this.world.find(findElement.bind(null, "lceve::EventDisplay")) ;
      // Enable the event navigation if possible
      console.log( "Navigation enabled ? ", this.eventMgr.enableNavigation ) ;
      this.byId('prevEvent').setEnabled( this.eventMgr.enableNavigation ) ;
      this.byId('nextEvent').setEnabled( this.eventMgr.enableNavigation ) ;
      // Setup callback on new event loaded
      if (this.eventMgr) {
        console.log("Event navigator loaded");
        var self = this;
        this.mgr.RefreshEventInfo = function() {
          console.log( "Navigation enabled ? ", self.eventMgr.enableNavigation ) ;
          self.byId('prevEvent').setEnabled( self.eventMgr.enableNavigation ) ;
          self.byId('nextEvent').setEnabled( self.eventMgr.enableNavigation ) ;
          self.showEventInfo();
        }
        self.showEventInfo();
      }
    },

    /// On web socket message
    /// Receives all messages, need to filter interesting messages
    OnWebsocketMsg : function(handle, msg, offset) {
      if ( typeof msg == "string") {
        // console.log( "received message: ", msg );
        if ( msg.substr(0,4) == "FW2_") {
          var resp = JSON.parse(msg.substring(4));
          var fnName = "addCollectionResponse";
          this[fnName](resp);
          return;
        }
      }
      this.mgr.OnWebsocketMsg(handle, msg, offset);
    },

    /// Send a message to the application to exit the ROOT application
    QuitRoot : function(oEvent) {
      this.mgr.SendMIR({
        "mir":        "QuitRoot()",
        "fElementId": this.eventDisplay.fElementId,
        "class":      "lceve::EventDisplay"
      });
    },

    /// Dummy help widget. TODO: implement a proper one
    showHelp : function(oEvent) {
      alert("=====User support: dummy@cern.ch");
    },

    /// Load the current event info on the corresponding widgets
    showEventInfo : function() {
      document.title = "LCEve: Run " + this.eventMgr.run + " / Event " + this.eventMgr.event ;
      this.byId("run-input").setValue(this.eventMgr.run);
      this.byId("event-input").setValue(this.eventMgr.event);
      this.byId("date-label").setText(this.eventMgr.date);
      this.byId("detector-input").setValue(this.eventMgr.detector);
    },

    /// Go to the next event
    nextEvent : function(oEvent) {
      console.log( "nextEvent" );
      this.mgr.SendMIR({
        "mir": "NextEvent()",
        "fElementId": this.eventMgr.fElementId,
        "class":      "lceve::EventNavigator"
      });
    },

    /// Go to the previous event
    prevEvent : function(oEvent) {
      this.mgr.SendMIR({
        "mir":        "PreviousEvent()",
        "fElementId": this.eventMgr.fElementId,
        "class":      "lceve::EventNavigator"
      });
    }
  });
});
