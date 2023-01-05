FROM python:3.8

COPY requirements.txt .

RUN pip install -r requirements.txt

ADD adapter.py /mqtt_adapter/
WORKDIR /mqtt_adapter/

CMD ["python3", "adapter.py"]