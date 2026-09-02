FROM golang:alpine AS builder

WORKDIR /build

COPY . .

RUN go build -o producer producer.go

FROM alpine

WORKDIR /build

COPY --from=builder /build/producer /build/producer

CMD [". /producer"]
