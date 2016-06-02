from __future__ import print_function
import requests
from datetime import timedelta, datetime, tzinfo, date

def sendWordPin(dict):
    print(dict)
    pin = {
        'id': 'wordOfTheDay-'+date.today().strftime("%m%d"),
        'time': datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ"),
        'layout': {
            'type': 'genericPin',
            'title': "Word of the Day",
            'subtitle': dict['word'],
            'body': dict['partOfSpeech']+'\n'+dict['definition'],
            'tinyIcon': 'system://images/HOTEL_RESERVATION'
        }
    }

    url = 'https://timeline-api.getpebble.com/v1/shared/pins/wordOfTheDay-'+date.today().strftime("%m%d")
    header = {
        'X-API-Key': 'SBtt4wrporypqzg1yb6bduzvwdryil31',
        'X-Pin-Topics': 'wordofday'
    }
    r = requests.put(url,headers=header,json=pin)
    print(r.text)

if __name__ == '__main__':
    url = "http://api.wordnik.com/v4/words.json/wordOfTheDay?api_key=a2a73e7b926c924fad7001ca3111acd55af2ffabf50eb4ae5"

    r = requests.get(url)

    try:
        r.json()
    except ValueError as e:
        print("JSON was invalid")
    else:
        word = {
            'word': r.json()['word'].capitalize(),
            'partOfSpeech': r.json()['definitions'][0]['partOfSpeech'].capitalize(),
            'definition': r.json()['definitions'][0]['text'].capitalize()
        }
        sendWordPin(word)
