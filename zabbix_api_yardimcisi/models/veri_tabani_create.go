package main

import (
	"database/sql"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"strconv"
	"strings"

	"github.com/PuerkitoBio/goquery"
	_ "github.com/lib/pq"
)

type MethodData struct {
	Method  string
	Headers []string
	Rows    [][]string
}

func main() {
	dbConfig, err := readDBConfig("db_config")
	if err != nil {
		log.Fatal(err)
	}

	methods := []string{"item", "trigger", "history", "trend", "graph", "event", "problem", "map", "task", "service", "host", "hostgroup", "template", "templategroup", "valuemap"}
	methodDataList := make([]MethodData, 0, len(methods))
	saveAllMethodsToDatabase(methods, dbConfig)
	for _, method := range methods {
		methodData := MethodData{
			Method: method,
		}
		url := fmt.Sprintf("https://www.zabbix.com/documentation/current/en/manual/api/reference/%s/get", method)

		response, err := http.Get(url)
		if err != nil {
			log.Fatal(err)
		}
		defer response.Body.Close()

		doc, err := goquery.NewDocumentFromReader(response.Body)
		if err != nil {
			log.Fatal(err)
		}

		headers, rows := parseTable(doc)
		filteredHeaders := filterHeaders(headers)
		filteredRows := filterRows(filteredHeaders, rows)

		methodData.Headers = filteredHeaders
		methodData.Rows = filteredRows

		methodDataList = append(methodDataList, methodData)

		saveToDatabase(methodData, dbConfig)
	}
}

func parseTable(doc *goquery.Document) ([]string, [][]string) {
	tableContainer := doc.Find(".table-container")
	table := tableContainer.Find("table")

	headers := make([]string, 0)
	rows := make([][]string, 0)

	table.Find("tr").Each(func(i int, row *goquery.Selection) {
		cells := make([]string, 0)
		row.Find("th, td").Each(func(j int, cell *goquery.Selection) {
			cells = append(cells, cell.Text())
		})
		if i == 0 {
			headers = filterHeaders(cells)
		} else {
			rows = append(rows, cells)
		}
	})

	return headers, rows
}

func filterHeaders(headers []string) []string {
	filteredHeaders := make([]string, 0)
	for _, header := range headers {
		if header != "Description" {
			filteredHeaders = append(filteredHeaders, header)
		}
	}
	return filteredHeaders
}

func filterRows(headers []string, rows [][]string) [][]string {
	filteredRows := make([][]string, 0)
	for _, cells := range rows {
		if len(cells) != len(headers) {
			continue // Geçersiz satırı atla
		}

		filteredCells := make([]string, 0)
		for i, cell := range cells {
			if headers[i] != "Description" {
				filteredCells = append(filteredCells, cell)
			}
		}
		filteredRows = append(filteredRows, filteredCells)
	}
	return filteredRows
}

func saveToDatabase(methodData MethodData, dbConfig DBConfig) {
	db := connectToDatabase(dbConfig)
	defer db.Close()

	createTableQuery := generateCreateTableQuery(methodData.Method, methodData.Headers)
	err := executeQuery(db, createTableQuery)
	if err != nil {
		log.Fatal(err)
	}

	for _, row := range methodData.Rows {
		// fmt.Print(row, "\n")
		insertQuery := generateInsertQuery(methodData.Method, methodData.Headers, row)
		err := executeQuery(db, insertQuery)
		if err != nil {
			log.Fatal(err)
		}
	}

	fmt.Printf("Veriler '%s' tablosuna kaydedildi.\n", methodData.Method)
}

func connectToDatabase(dbConfig DBConfig) *sql.DB {
	psqlInfo := fmt.Sprintf("host=%s port=%d user=%s "+
		"password=%s dbname=%s sslmode=disable",
		dbConfig.Host, dbConfig.Port, dbConfig.User, dbConfig.Password, dbConfig.DBName)
	db, err := sql.Open("postgres", psqlInfo)
	if err != nil {
		log.Fatal(err)
	}
	return db
}

func executeQuery(db *sql.DB, query string) error {
	_, err := db.Exec(query)
	return err
}

func generateCreateTableQuery(tableName string, headers []string) string {
	columns := generateTableColumns(headers)
	return fmt.Sprintf("CREATE TABLE IF NOT EXISTS %s (%s);", tableName, columns)
}

func generateInsertQuery(tableName string, headers []string, row []string) string {
	columnNames := strings.Join(headers, ", ")
	columnValues := make([]string, len(row))
	for i, value := range row {
		columnValues[i] = "'" + value + "'"
	}
	return fmt.Sprintf("INSERT INTO %s (%s) VALUES (%s);", tableName, columnNames, strings.Join(columnValues, ", "))
}

func generateTableColumns(headers []string) string {
	columns := make([]string, len(headers))
	for i, header := range headers {
		columns[i] = fmt.Sprintf(`"%s" VARCHAR(100)`, strings.ToLower(header))
	}
	return strings.Join(columns, ", ")
}

func saveAllMethodsToDatabase(methods []string, dbConfig DBConfig) {
	db := connectToDatabase(dbConfig)
	defer db.Close()

	tableName := "all_methods"
	headers := []string{"method"}

	// Tabloyu oluşturma sorgusu
	createTableQuery := generateCreateTableQuery(tableName, headers)
	err := executeQuery(db, createTableQuery)
	if err != nil {
		log.Fatal(err)
	}

	// Verileri tabloya ekleme
	for _, method := range methods {
		row := []string{method}
		insertQuery := generateInsertQuery(tableName, headers, row)
		err := executeQuery(db, insertQuery)
		if err != nil {
			log.Fatal(err)
		}
	}

	fmt.Printf("Veriler '%s' tablosuna kaydedildi.\n", tableName)
}

type DBConfig struct {
	Host     string
	Port     int
	User     string
	Password string
	DBName   string
}

func readDBConfig(filename string) (DBConfig, error) {
	var config DBConfig

	content, err := ioutil.ReadFile(filename)
	if err != nil {
		return config, err
	}

	lines := strings.Split(string(content), "\n")
	for _, line := range lines {
		parts := strings.SplitN(line, "=", 2)
		if len(parts) == 2 {
			key := strings.TrimSpace(parts[0])
			value := strings.TrimSpace(parts[1])
			switch key {
			case "host":
				config.Host = value
			case "port":
				config.Port = parseInt(value)
			case "user":
				config.User = value
			case "password":
				config.Password = value
			case "dbname":
				config.DBName = value
			}
		}
	}

	return config, nil
}

func parseInt(s string) int {
	val, _ := strconv.Atoi(s)
	return val
}
