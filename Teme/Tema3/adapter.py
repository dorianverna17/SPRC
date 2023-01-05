import paho.mqtt.client as mqtt
import influxdb_client
import time

from influxdb_client.client.write_api import SYNCHRONOUS

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("#")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    pass
    #print(msg.topic+" "+str(msg.payload))

url="database_influx"
client_database = influxdb_client.InfluxDBClient(url=url)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("mqtt.eclipseprojects.io", 1883, 60)
client.loop_start()

time.sleep(1)
while True:
    time.sleep(1)

client.loop_stop()
