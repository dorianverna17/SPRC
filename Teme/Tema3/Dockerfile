FROM python:3.8

COPY requirements.txt .

RUN pip install paho.mqtt.client
RUN pip install influxdb_client

ADD adapter.py /mqtt_adapter/
WORKDIR /mqtt_adapter/

CMD ["python3", "adapter.py"]