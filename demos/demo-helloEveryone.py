from __future__ import print_function
import requests
from datetime import timedelta, datetime, tzinfo, date

def sendPin():
    pin = {
        'id': 'pebbleNYCHello',
        'time': datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ"),
        'layout': {
            'type': 'genericPin',
            'title': 'Hello, PebbleNYC',
            'body': 'Hello Everybody.',
            'tinyIcon': 'system://images/HOTEL_RESERVATION'
        },
        "createNotification": {
            "layout": {
                "type": "genericNotification",
                "title": "Hello, PebbleNYC",
                "tinyIcon": 'system://images/HOTEL_RESERVATION'
            }
        }
    }

    url = 'https://timeline-api.getpebble.com/v1/shared/pins/pebbleNYCHello'
    header = {
        'X-API-Key': 'SBtt4wrporypqzg1yb6bduzvwdryil31',
        'X-Pin-Topics': 'all-users'
    }
    r = requests.put(url,headers=header,json=pin)
    print(r.text)

if __name__ == '__main__':
    sendPin()
