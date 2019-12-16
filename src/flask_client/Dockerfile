FROM python:3.7-alpine

ENV DATABASE_FILE /db/coffeere.sqlite3

RUN apk add gcc musl-dev libffi-dev openssl-dev python3-dev openssl

MAINTAINER Maxwell "maxwell@lsd.ufcg.edu.br"

COPY . /app
WORKDIR /app

RUN sh generate_key.sh 
RUN pip install -r requirements.txt

CMD ["python", "main.py"]
