FROM alpine:latest

RUN apk update && apk add python3 py3-pip
RUN python3 -m pip install requests

COPY main.py .

CMD ["python3", "main.py"]
