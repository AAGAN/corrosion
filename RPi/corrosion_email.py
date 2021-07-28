import subprocess
import smtplib
from email.mime.text import MIMEText
import RPi.GPIO as GPIO
import random
import time
import datetime

GPIO.setmode(GPIO.BOARD)

#def connect_type(word_list):
#    """ This function takes a list of words, then, depeding which key word, returns the corresponding
#    internet connection type as a string. ie) 'ethernet'.
#    """
#    if 'wlan0' in word_list or 'wlan1' in word_list:
#        con_type = 'wifi'
#    elif 'eth0' in word_list:
#        con_type = 'ethernet'
#    else:
#        con_type = 'current'
#    return con_type

gpio_input_pins = [7,11,12,13,15,16,18,22,29,31,32,33,35,36,37,38,40]
gpio_output_pins = []
gpio_triggered_pins = []

for i in gpio_input_pins:
    GPIO.setup(i, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

for i in gpio_output_pins:
    GPIO.setup(i, GPIO.OUT)

fromemail = 'tyco.physics@gmail.com'
password = 'Tyco1234'
toemail = 'aahmadzadegan@gmail.com'

def sendemail(from_email, from_password, to_email, title_text, body_text):
    msg = MIMEText(body_text)
    msg['subject'] = title_text
    msg['from'] = from_email 
    msg['to'] = to_email
    smtpserver = smtplib.SMTP('smtp.gmail.com', 587) # Server to use.
    smtpserver.ehlo()  # Says 'hello' to the server
    smtpserver.starttls()  # Start TLS encryption
    smtpserver.login(from_email, from_password)  # Log in to server
    smtpserver.sendmail(msg['From'], msg['To'], msg.as_string())
    smtpserver.quit()

today = datetime.date.today()  # Get current time/date
arg='ip route list'  # Linux command to retrieve ip addresses.
# Runs 'arg' in a 'hidden terminal'.
p=subprocess.Popen(arg,shell=True,stdout=subprocess.PIPE)
data = p.communicate()  # Get data from 'p terminal'.

# Split IP text block into three, and divide the two containing IPs into words.
ip_lines = data[0].splitlines()
split_line_a = ip_lines[0].split()
split_line_b = ip_lines[1].split()

# con_type variables for the message text. ex) 'ethernet', 'wifi', etc.
ip_type_a = split_line_b[8]
#ip_type_b = connect_type(split_line_b)

"""Because the text 'src' is always followed by an ip address,
we can use the 'index' function to find 'src' and add one to
get the index position of our ip.
"""
#ipaddr_a = split_line_a[split_line_a.index('src')+1]
#ipaddr_b = split_line_b[split_line_b.index('src')+1]

# Creates a sentence for each ip address.
my_ip_a = 'Your ip is %s' % ip_type_a
#my_ip_b = 'Your %s ip is %s' % (ip_type_b, ipaddr_b)

# Creates the text, subject, 'from', and 'to' of the message.    
message1 = my_ip_a + "\n" #+ my_ip_b + "\n \n"
subject1 = 'IPs and initial conditions on %s' %today.strftime('%b %d %Y')    
    
#message = ""
for i in gpio_input_pins:
    message1 += "pin " + str(i) +" is "+ str(GPIO.input(i)) + "\n"
    time.sleep(0.1)
sendemail(fromemail, password, toemail, subject1, message1)	
	
while True:
    message  = ""
    for i in gpio_input_pins:
        if not(GPIO.input(i)):
            try:
                gpio_triggered_pins.remove(i)
            except:
                pass
        else:
            if gpio_triggered_pins.count(i)==0:
                gpio_triggered_pins.append(i)
                message += "pin " + str(i) + " is high now! " + "\n"
    if message != "":
        sendemail(fromemail,password,toemail,"something happened!",message)
    time.sleep(30)


GPIO.cleanup()
