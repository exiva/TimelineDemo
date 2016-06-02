var getTopic = function (number) {
  switch (number) {
    case 0:
      return "wordofday"
    break;
  }
}

var timeline_token;

var pin = {
  "duration": 60,
  "layout": {
    "type": "calendarPin",
    "title": "Reservation at nobu",
    "body": "Table for 2",
    "tinyIcon": "system://images/DINNER_RESERVATION"
  }
};

var timelinePin = function timelinePin(method, id, pin) {
  var req = new XMLHttpRequest();

  pin['id'] = id;

  req.open(method, 'https://timeline-api.getpebble.com/v1/user/pins/' + id, true);
  req.setRequestHeader('Content-Type', 'application/json');
  req.setRequestHeader('X-User-Token', timeline_token);

  req.onload = function complete() {
    if (req.readyState === 4) {
      if (req.status === 200) {
        console.log(req.responseText);
      } else {
        console.log(req.responseText);
      }
    }
  };

  console.log(JSON.stringify(pin));
  req.timeout = 15000;
  req.send(JSON.stringify(pin));
};

Pebble.addEventListener("ready", function(e) {
  Pebble.timelineSubscribe('all-users', function (e) {
    console.log("Subscribed to generic allusers");
  });

  Pebble.getTimelineToken(function(token) {
    console.log('Timeline Token: ' + token);
    timeline_token = token;
  }, function(error) {
    console.log('Error getting timeline token: ' + error);
  });
});

Pebble.addEventListener("appmessage", function(e) {
  var message = e.payload;
  var subscribed = Boolean(message.STATUS);
  var dict = {};
  console.log("Incoming Message: "+JSON.stringify(e.payload));
  if (message.GROUP === 0) {
    console.log("Local Pin Actions");
    if (message.SUB === 0) {
      console.log("send pin");
      datetime = new Date();
      datetime.setMinutes(datetime.getMinutes() + 15);
      pin.time = datetime.toISOString();

      timelinePin('PUT', 'demo-HelloWorld', pin);
    } else if (message.SUB === 1) {
      pin.layout.title = "Reservation at Natsumi";
      datetime = new Date();
      datetime.setHours(datetime.getHours() + 1);
      pin.time = datetime.toISOString();
      pin['updateNotification'] = {
        'time': datetime,
        'layout': {
          'type': 'genericNotification',
          'title': 'Reservation Changed',
          'tinyIcon': 'system://images/DINNER_RESERVATION',
          'body': 'Your reservation was changed to Natsumi and is now at' + datetime.getHours()
        }
      }
      // console.log(JSON.stringify(pin));
      timelinePin('PUT', 'demo-HelloWorld', pin);
    } else {
      timelinePin('DELETE', 'demo-HelloWorld', null);
    }
  } else {
    if (!subscribed) {
      Pebble.timelineSubscribe(getTopic(message.SUB), function (e) {
        console.log("Subscribed to "+getTopic(message.SUB));
        dict['SUB'] = message.SUB;
        dict['STATUS'] = 1;
        Pebble.sendAppMessage(dict);
      });
    } else {
      Pebble.timelineUnsubscribe(getTopic(message.SUB), function (e) {
        console.log("Unsubscribed from "+getTopic(message.SUB));
        dict['SUB'] = message.SUB;
        dict['STATUS'] = 0;
        Pebble.sendAppMessage(dict);
      });
    }
  }
});
