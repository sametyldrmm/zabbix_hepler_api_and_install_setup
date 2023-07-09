package main

import (
	"bytes"
	"database/sql"
	"encoding/csv"
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

	config, err := readEnvFile(envFile)
	if err != nil {
		fmt.Println(err)
		return
	}

	format, method, itemParams, err := getItemParamsFromArguments(os.Args)
	if err != nil {
		fmt.Println(err)
		return
	}

	itemGetRequest := createItemGetRequest(method, itemParams)

	response, err := sendRequest(config, itemGetRequest)
	if err != nil {
		fmt.Println(err)
		return
	}
	printterResponse(response, format)
}

func printterResponse(response map[string]interface{}, format string) {
	// json formatı
	if format == "json" {
		jsonData, err := json.MarshalIndent(response, "", "  ")
		if err != nil {
			fmt.Println("JSON oluşturulamadı:", err)
			return
		}
		fmt.Println(string(jsonData))
	}
	if format == "csv" {
		// CSV formatına dönüştürme işlemleri
		csvData, err := convertToCSV(response["result"])
		if err != nil {
			fmt.Println("CSV oluşturulamadı:", err)
			return
		}
		fmt.Println(csvData)
	}
	// tablo formatı
	if format == "table" {
		processResponse(response)
	}

}
func readEnvFile(file string) (Config, error) {
	envData, err := ioutil.ReadFile(file)
	if err != nil {
		return Config{}, err
	}

	envLines := strings.Split(string(envData), "\n")

	return Config{
		URL:           strings.TrimSpace(envLines[0]),
		Method:        strings.TrimSpace(envLines[1]),
		Authorization: strings.TrimSpace(envLines[2]),
		ContentType:   strings.TrimSpace(envLines[3]),
	}, nil
}

func getItemParamsFromArguments(args []string) (string, string, interface{}, error) {
	method, err := extractMethodFromArguments(args)
	if err != nil {
		return "", "", nil, err
	}

	methodParameter, err := getMethodParameterFromDB(method)
	if err != nil {
		return "", "", nil, fmt.Errorf("Veritabanı bağlantı bilgileri okunamadı.")
	}

	formatter, itemParams, err := extractParamsFromArguments(args, methodParameter)
	if err != nil {
		return "", "", nil, err
	}

	return formatter, method, itemParams, nil
}

func extractMethodFromArguments(args []string) (string, error) {
	if len(args) < 2 {
		return "", fmt.Errorf("Geçersiz argüman sayısı")
	}

	arg := args[1]
	parts := strings.SplitN(arg, ":", 2)
	if len(parts) != 2 || parts[0] != "method" {
		return "", fmt.Errorf("Geçersiz argüman formatı: %s", arg)
	}

	return strings.TrimSpace(parts[1]), nil
}

func extractParamsFromArguments(args []string, methodParameter MethodParameter) (string, map[string]interface{}, error) {
	itemParams := make(map[string]interface{})
	formatter := "table"
	for _, arg := range args[2:] {
		key, value, err := extractKeyValueFromArgument(arg)
		if err != nil {
			return formatter, nil, err
		}

		switch key {
		case "format":
			formatter = value
		case FindString(methodParameter.Query, key):
			itemParams[FindString(methodParameter.Query, key)] = strings.Split(value, ",")
		case FindString(methodParameter.Integer, key):
			itemParams[FindString(methodParameter.Integer, key)] = parseInt(value)
		case FindString(methodParameter.String, key):
			itemParams[FindString(methodParameter.String, key)] = strings.Split(value, ",")
		case FindString(methodParameter.Boolen, key):
			itemParams[FindString(methodParameter.Boolen, key)] = (value == "true")
		case FindString(methodParameter.Object, key):
			itemParams[FindString(methodParameter.Object, key)] = getJson(value)
		default:
			return formatter, nil, fmt.Errorf("Bilinmeyen argüman: %s", key)
		}
	}

	return formatter, itemParams, nil
}

func extractKeyValueFromArgument(arg string) (string, string, error) {
	parts := strings.SplitN(arg, ":", 2)
	if len(parts) != 2 {
		return "", "", fmt.Errorf("Geçersiz argüman formatı: %s", arg)
	}

	return strings.TrimSpace(parts[0]), strings.TrimSpace(parts[1]), nil
}

func createItemGetRequest(method string, params interface{}) ItemGetRequest {
	return ItemGetRequest{
		JSONRPC: "2.0",
		Method:  method,
		Params:  params,
		ID:      1,
	}
}

func sendRequest(config Config, request ItemGetRequest) (map[string]interface{}, error) {
	payload := strings.NewReader(removeBackslash(request.String()))

	client := &http.Client{}
	req, err := createHTTPRequest(config.Method, config.URL, payload)
	if err != nil {
		return nil, err
	}

	addRequestHeaders(req, config.Authorization, config.ContentType)

	res, err := executeRequest(client, req)
	if err != nil {
		return nil, err
	}
	defer res.Body.Close()

	body, err := readResponseBody(res)
	if err != nil {
		return nil, err
	}

	response, err := parseResponse(body)
	if err != nil {
		return nil, err
	}

	return response, nil
}

func createHTTPRequest(method, url string, payload *strings.Reader) (*http.Request, error) {
	req, err := http.NewRequest(method, url, payload)
	if err != nil {
		return nil, err
	}
	return req, nil
}

func addRequestHeaders(req *http.Request, authorization, contentType string) {
	req.Header.Add("Authorization", authorization)
	req.Header.Add("Content-Type", contentType)
}

func executeRequest(client *http.Client, req *http.Request) (*http.Response, error) {
	res, err := client.Do(req)
	if err != nil {
		return nil, err
	}
	return res, nil
}

func readResponseBody(res *http.Response) ([]byte, error) {
	body, err := ioutil.ReadAll(res.Body)
	if err != nil {
		return nil, err
	}
	return body, nil
}

func parseResponse(body []byte) (map[string]interface{}, error) {
	var response map[string]interface{}
	err := json.Unmarshal(body, &response)
	if err != nil {
		return nil, err
	}
	return response, nil
}

func processResponse(response map[string]interface{}) {
	if isError(response) {
		handleError(response)
		return
	}

	if resultData, ok := extractResultData(response); ok {
		if len(resultData) == 0 {
			fmt.Println("Sonuç bulunamadı.")
			return
		}

		headers := extractHeaders(resultData)
		// tablePrint(resultData)
		printTable(headers, resultData)
	}
}

func isError(response map[string]interface{}) bool {
	_, ok := response["error"]
	return ok
}

func handleError(response map[string]interface{}) {
	errorData := response["error"].(map[string]interface{})
	fmt.Printf("Hata: %v\n", errorData["message"])
}

func extractResultData(response map[string]interface{}) ([]map[string]interface{}, bool) {
	resultData, ok := response["result"].([]interface{})
	if !ok {
		return nil, false
	}

	data := make([]map[string]interface{}, len(resultData))
	for i, item := range resultData {
		data[i] = item.(map[string]interface{})
	}

	return data, true
}

func extractHeaders(resultData []map[string]interface{}) []string {
	if len(resultData) == 0 {
		return nil
	}

	firstItem := resultData[0]
	headers := make([]string, 0, len(firstItem))
	for key := range firstItem {
		headers = append(headers, key)
	}

	return headers
}

func printTable(headers []string, resultData []map[string]interface{}) {
	w := tabwriter.NewWriter(os.Stdout, 0, 0, 1, ' ', tabwriter.TabIndent)
	fmt.Fprintln(w, strings.Join(headers, "\t"))

	// Verileri tabloya ekle
	for _, item := range resultData {
		row := make([]string, 0)
		for _, header := range headers {
			value := fmt.Sprintf("%v", item[header])
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

func getMethodParameterArgClassificition(table []map[string]interface{}) MethodParameter {
	var methodParameter MethodParameter
	for _, row := range table {
		switch row["type"] {
		case "string/array":
			methodParameter.String = append(methodParameter.String, row["parameter"].(string))
		case "integer":
			methodParameter.Integer = append(methodParameter.Integer, row["parameter"].(string))
		case "boolean":
			methodParameter.Boolen = append(methodParameter.Boolen, row["parameter"].(string))
		case "array of objects":
			methodParameter.ArrayOfObject = append(methodParameter.ArrayOfObject, row["parameter"].(string))
		case "object":
			methodParameter.Object = append(methodParameter.Object, row["parameter"].(string))
		case "query":
			methodParameter.Query = append(methodParameter.Query, row["parameter"].(string))
		}
	}
	return methodParameter
}

func getJson(data string) string {
	result := parseStringToJSON(data)
	jsonBytes, err := json.Marshal(result)

	if err != nil {
		fmt.Println("JSON marshaling error:", err)
		return ""
	}

	jsonString := string(jsonBytes)
	return jsonString
}

func parseStringToJSON(input string) map[string]interface{} {
	result := make(map[string]interface{})

	parts := strings.Split(input, "0")
	for _, part := range parts {
		keyValue := strings.Split(part, ":")
		key := keyValue[0]
		args := keyValue[1]
		result[key] = args
	}

	return result
}

func removeBackslash(str string) string {
	result := strings.ReplaceAll(str, "\\\"", "\"")
	result = strings.ReplaceAll(result, "\"{", "{")
	result = strings.ReplaceAll(result, "}\"}", "}}")
	result = strings.ReplaceAll(result, "\"}\"", "\"}")
	return result
}

func FindString(strs []string, target string) string {
	for _, str := range strs {
		if str == target {
			return str
		}
	}
	return "" // Eğer bulunamazsa boş bir dize döndürür
}

func getMethodParameterFromDB(method string) (MethodParameter, error) {
	dbConfig, err := readDBConfig("./models/db_config")
	if err != nil {
		return MethodParameter{}, fmt.Errorf("Veritabanı bağlantı bilgileri okunamadı.")
	}

	db := connectToDatabase(dbConfig)
	if db == nil {
		return MethodParameter{}, fmt.Errorf("Veritabanı bağlantısı kurulamadı.")
	}
	defer disconnectFromDatabase(db)

	data, err := getDbTable(db, strings.Split(method, ".")[0])
	if err != nil {
		return MethodParameter{}, err
	}

	return getMethodParameterArgClassificition(data), nil
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

func convertToCSV(data interface{}) (string, error) {

	items, ok := data.([]interface{})
	if !ok {
		return "", fmt.Errorf("veri dilim (slice) türünde değil")
	}

	var csvData bytes.Buffer
	csvWriter := csv.NewWriter(&csvData)
	var header []string
	if len(items) > 0 {
		itemMap, ok := items[0].(map[string]interface{})
		if !ok {
			return "", fmt.Errorf("öğe harita (map) türünde değil")
		}

		for field := range itemMap {
			header = append(header, field)
		}

		err := csvWriter.Write(header)
		if err != nil {
			return "", err
		}
	}

	for _, item := range items {
		itemMap, ok := item.(map[string]interface{})
		if !ok {
			return "", fmt.Errorf("öğe harita (map) türünde değil")
		}
		var row []string
		for _, value := range itemMap {
			row = append(row, fmt.Sprintf("%v", value))
		}
		err := csvWriter.Write(row)
		if err != nil {
			return "", err
		}
	}

	csvWriter.Flush()

	if err := csvWriter.Error(); err != nil {
		return "", err
	}
	return csvData.String(), nil
}
