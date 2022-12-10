FROM python:3.8

COPY requirements.txt .

RUN pip install -r requirements.txt

ADD rest_api/server.py /rest_api/
WORKDIR /rest_api

EXPOSE 6000

CMD ["python3", "server.py"]