FROM golang:alpine AS builder

WORKDIR /build

COPY . .

#RUN go mod init consumer
#RUN go mod tidy

RUN go build -o consumer consumer.go

FROM alpine

WORKDIR /build

COPY --from=builder /build/consumer /build/consumer

CMD [". /consumer"]
