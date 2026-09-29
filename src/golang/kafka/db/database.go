package database

import (
	"context"
	"fmt"
	"log"
	"os"

	"github.com/jackc/pgx/v5/pgxpool"
)

type ButtonsEvents struct {
	buttonId   int
	buttonName string
	clicksCnt  int
}

var pool *pgxpool.Pool

func getConfigFromEnvs() (pgxpool.Config, error) {
	var config pgxpool.Config

	host := os.Getenv("POSTGRES_HOST")
	port := os.Getenv("POSTGRES_PORT")
	dbName := os.Getenv("POSTGRES_DB")
	user := os.Getenv("POSTGRES_USER")
	pass := os.Getenv("POSTGRES_PASSWORD")
	maxConns := os.Getenv("MAX_CONNECTIONS")
	connStr := fmt.Sprintf("postgres://%s:%s@%s:%s/%s", user, pass, host, port, dbName)

	config, err := pgxpool.ParseConfig(connStr)
	if err != nil {
		log.Fatal("Ошибка конфигурации пула: ", err)
	}
	config.MaxConns = maxConns

	return config, nil
}

func InitFromEnvs() error {
	config, err := getConfigFromEnvs()
	if err != nil {
		return err
	}
	pool, err = pgxpool.NewWithConfig(context.Background(), config)
	if err != nil {
		log.Fatal("Ошибка создания пула: ", err)
	}
	return nil
}

func ClosePool() {
	if pool != nil {
		err := pool.Close()
		if err != nil {
			log.Println("Ошибка при закрытии подключения к БД:", err)
		} else {
			log.Println("Подключение к БД закрыто:", err)
		}
	}
}

func Healthcheck() bool {
	connection, err := pool.Acquire(context.Background())
	if err != nil {
		return false
	}
	defer connection.Release()

	err = connection.Ping(context.Background())
	if err != nil {
		return false
	} else {
		return true
	}
}

func StoreButtonInfo(ctx context.Context, bi ButtonsEvents) error {
	connection, err := pool.Acquire(ctx)
	if err != nil {
		return err
	}
	defer connection.Release()

	tx := connection.Begin(ctx)

	buttonIdRow := tx.QueryRow(ctx, "SELECT id FROM ButtonsInfo", bi.buttonId)
	var id int64
	err = buttonIdRow.Scan(&id)
	if err != nil {
		_, err = tx.Exec(ctx, "INSERT INTO ButtonsInfo VALUES(?, ?, ?)", bi.buttonId, bi.buttonName, bi.clicksCnt)
		if err != nil {
			tx.Rollback(ctx)
			return err
		}
	} else {
		_, err = tx.Exec(ctx, "UPDATE ButtonsInfo SET activity = activity + ? WHERE id = ?", bi.clicksCnt, bi.buttonId)
		if err != nil {
			tx.Rollback(ctx)
			return err
		}
	}

	tx.Commit(ctx)
	return nil
}

//(pool *pgxpool.Pool)
