#!		/usr/bin/env python3
import os
import http.cookies

# Function to set a cookie
def set_cookie():
    # Create a cookie object
    cookie = http.cookies.SimpleCookie()
    
    # Set a cookie named "username" with the value "JohnDoe"
    cookie['username'] = 'JohnDoe'
    
    # Set additional cookie attributes (optional)
    cookie['username']['expires'] = 3600  # Cookie expires in 1 hour
    cookie['username']['path'] = '/'
    
    # Send the Content-Type header with a Set-Cookie header
    print("Content-Type: text/html")
    print(cookie.output())
    print()  # Print an empty line to indicate the start of the HTML content

    # Print a message
    print("<html><body>")
    print("<h1>Cookie Set</h1>")
    print("<p>Cookie 'username' has been set with the value 'JohnDoe'.</p>")
    print("</body></html>")

# Function to get and display a cookie
def get_cookie():
    # Read the HTTP_COOKIE environment variable to get the cookie data
    cookie_str = os.environ.get('HTTP_COOKIE', '')

    # Parse the cookie string into a dictionary
    cookies = http.cookies.SimpleCookie()
    cookies.load(cookie_str)

    # Check if the "username" cookie exists
    if 'username' in cookies:
        username = cookies['username'].value
    else:
        username = 'Cookie not found'

    # Send the Content-Type header
    print("Content-Type: text/html")
    print()  # Print an empty line to indicate the start of the HTML content

    # Print a message with the retrieved cookie value
    print("<html><body>")
    print("<h1>Cookie Value</h1>")
    print(f"<p>Value of the 'username' cookie: {username}</p>")
    print("</body></html>")

# Check the query string to determine the action
query_string = os.environ.get('QUERY_STRING', '')

if query_string == 'set':
    set_cookie()
elif query_string == 'get':
    get_cookie()
else:
    # Default behavior: Show links to set and get cookies
    print("Content-Type: text/html")
    print()
    print("<html><body>")
    print("<h1>Cookie Example</h1>")
    print("<p><a href='cookie_handler.cgi?set'>Set Cookie</a></p>")
    print("<p><a href='cookie_handler.cgi?get'>Get Cookie</a></p>")
    print("</body></html>")
