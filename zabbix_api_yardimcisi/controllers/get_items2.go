package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"strconv"
	"strings"
	"text/tabwriter"

	_ "github.com/lib/pq"
)

type ItemGetRequest struct {
	JSONRPC string      `json:"jsonrpc"`
	Method  string      `json:"method"`
	Params  interface{} `json:"params,omitempty"`
	ID      int         `json:"id"`
}

type Config struct {
	URL           string
	Method        string
	Authorization string
	ContentType   string
}

type MethodParameter struct {
	Boolen        []string
	String        []string
	Integer       []string
	ArrayOfObject []string
	Query         []string
	Object        []string
}

type DBConfig struct {
	Host     string
	Port     int
	User     string
	Password string
	DBName   string
}

func main() {
	envFile := "env.txt"
	var method_str string
	// Değerleri env.txt dosyasından oku
	config, err := readEnvFile(envFile)
	if err != nil {
		fmt.Println(err)
		return
	}

	method_str, itemParams, err := getItemParamsFromArguments(os.Args)
	if err != nil {
		fmt.Println(err)
		return
	}

	itemGetRequest := ItemGetRequest{
		JSONRPC: "2.0",
		Method:  method_str,
		Params:  itemParams,
		ID:      1,
	}

	payload := strings.NewReader(itemGetRequest.String())

	client := &http.Client{}
	req, err := http.NewRequest(config.Method, config.URL, payload)
	if err != nil {
		fmt.Println(err)
		return
	}
	req.Header.Add("Authorization", config.Authorization)
	req.Header.Add("Content-Type", config.ContentType)

	res, err := client.Do(req)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer res.Body.Close()

	body, err := ioutil.ReadAll(res.Body)
	if err != nil {
		fmt.Println(err)
		return
	}

	var response map[string]interface{}
	err = json.Unmarshal(body, &response)
	if err != nil {
		fmt.Println(err)
		return
	}

	if errorData, ok := response["error"].(map[string]interface{}); ok {
		fmt.Printf("Hata: %v\n", errorData["message"])
		return
	}

	if resultData, ok := response["result"].([]interface{}); ok {
		if len(resultData) == 0 {
			fmt.Println("Sonuç bulunamadı.")
			return
		}

		// Sütun başlıklarını al
		headers := make([]string, 0)
		firstItem := resultData[0].(map[string]interface{})
		for key := range firstItem {
			headers = append(headers, key)
		}

		// Tabloyu oluştur
		w := tabwriter.NewWriter(os.Stdout, 0, 0, 1, ' ', tabwriter.TabIndent)
		fmt.Fprintln(w, strings.Join(headers, "\t"))

		// Verileri tabloya ekle
		for _, item := range resultData {
			itemData := item.(map[string]interface{})
			row := make([]string, 0)
			for _, header := range headers {
				value := fmt.Sprintf("%v", itemData[header])
				if value == "" {
					value = "NULL"
				}
				row = append(row, value)
			}
			fmt.Fprintln(w, strings.Join(row, "\t"))
		}

		// Tabloyu yazdır
		w.Flush()
	}
}

func readEnvFile(file string) (Config, error) {
	envData, err := ioutil.ReadFile(file)
	if err != nil {
		return Config{}, err
	}

	envLines := strings.Split(string(envData), "\n")

	// URL değerini oku
	url := strings.TrimSpace(envLines[0])

	// Method değerini oku
	method := strings.TrimSpace(envLines[1])

	// Authorization değerini oku
	authorization := strings.TrimSpace(envLines[2])

	// Content-Type değerini oku
	contentType := strings.TrimSpace(envLines[3])

	config := Config{
		URL:           url,
		Method:        method,
		Authorization: authorization,
		ContentType:   contentType,
	}

	return config, nil
}

func if_len(values []string) []string {
	if values != nil && len(values) > 0 {
		return values
	}
	return nil
}

func getItemParamsFromArguments(args []string) (string, interface{}, error) {
	var method_str string

	itemParams := make(map[string]interface{})

	for _, arg := range args[1:] {
		parts := strings.SplitN(arg, ":", 2)
		if len(parts) != 2 {
			return "", nil, fmt.Errorf("Geçersiz argüman formatı: %s", arg)
		}

		key := strings.TrimSpace(parts[0])
		value := strings.TrimSpace(parts[1])
		if key == "method" {
			method_str = value
			dbConfig, err := readDBConfig("./models/db_config")
			if err != nil {
				fmt.Println(err)
				return "", nil, fmt.Errorf("Veritabanı bağlantı bilgileri okunamadı.")
			}
			fmt.Print("Veritabanı bağlantısı kuruluyor...")
			db := connectToDatabase(dbConfig)

			if db == nil {
				return "", nil, fmt.Errorf("Veritabanı bağlantısı kurulamadı.")
			}
			fmt.Print("Veritabanı bağlan")
			data, err := getDbTable(db, "nn")
			fmt.Println(data)
			disconnectFromDatabase(db)
			continue
		}
		var que_argüments = []string{"hostids", "output", "itemids"}
		var int_argüments = []string{"limit"}

		switch key {
		case FindString(que_argüments, key):
			itemParams[FindString(que_argüments, key)] = strings.Split(value, ",")
		case FindString(int_argüments, key):
			itemParams[FindString(int_argüments, key)] = value
		default:
			return "", nil, fmt.Errorf("Bilinmeyen argüman: %s", key)
		}
	}

	return method_str, itemParams, nil
}

func (r ItemGetRequest) String() string {
	b := strings.Builder{}
	b.WriteString(`{`)
	b.WriteString(fmt.Sprintf(`"jsonrpc": "%s",`, r.JSONRPC))
	b.WriteString(fmt.Sprintf(`"method": "%s",`, r.Method))
	if r.Params != nil {
		paramsJSON, _ := json.Marshal(r.Params)
		b.WriteString(fmt.Sprintf(`"params": %s,`, paramsJSON))
	}
	b.WriteString(fmt.Sprintf(`"id": %d`, r.ID))
	b.WriteString(`}`)
	return b.String()
}

func FindString(strs []string, target string) string {
	for _, str := range strs {
		if str == target {
			return str
		}
	}
	return "" // Eğer bulunamazsa boş bir dize döndürür
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

func getDbTable(db *sql.DB, table string) ([]map[string]interface{}, error) {
	rows, err := db.Query(fmt.Sprintf("SELECT * FROM %s", table))
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	columns, err := rows.Columns()
	if err != nil {
		return nil, err
	}

	var result []map[string]interface{}
	for rows.Next() {
		columnValues := make([]interface{}, len(columns))
		columnPointers := make([]interface{}, len(columns))
		for i := range columns {
			columnPointers[i] = &columnValues[i]
		}
		if err := rows.Scan(columnPointers...); err != nil {
			return nil, err
		}
		item := make(map[string]interface{})
		for i, columnName := range columns {
			item[columnName] = columnValues[i]
		}
		result = append(result, item)
	}

	return result, nil
}

func disconnectFromDatabase(db *sql.DB) {
	db.Close()
}

func tablePrint(table []map[string]interface{}) {
	w := tabwriter.NewWriter(os.Stdout, 0, 0, 1, ' ', tabwriter.Debug)
	for _, row := range table {
		for _, column := range row {
			fmt.Fprintf(w, "%v\t", column)
		}
		fmt.Fprintln(w)
	}
	w.Flush()
}
