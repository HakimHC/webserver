import requests

PORT_WEBSERV = 9292
PORT_NGINX = 8080

def test_status_code(uri):
    url_webserv = f"http://localhost:{PORT_WEBSERV}{uri}"
    url_nginx = f"http://localhost:{PORT_NGINX}{uri}"
    try:
        response_webserv = requests.get(url_webserv)
    except:
        print("fatal: cannot establish connection with webserv")
        return
    try:
        response_nginx = requests.get(url_nginx)
    except:
        print("fatal: cannot establish connection with nginx")
        return

    if response_nginx.status_code != response_webserv.status_code:
        print("KO")
    else:
        print("OK")

def test_all(uri):
    test_status_code(uri)

test_all("/")
test_all("/index")
test_all("/index/")
