from __future__ import with_statement
from urllib.parse import urlencode
from urllib.request import urlopen
import contextlib
import sys

def tinyurl(url):
    request_url = 'https://tinyurl.com/api-create.php?' + urlencode({'url': url})
    with contextlib.closing(urlopen(request_url)) as response:
        return response.read().decode('utf-8')

def main():
    for shorturl in map(tinyurl, sys.argv[1:]):
        print(shorturl)

if __name__ == '__main__':
    main()
