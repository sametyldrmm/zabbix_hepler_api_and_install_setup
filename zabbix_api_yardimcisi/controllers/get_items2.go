package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"strings"
	"text/tabwriter"
)

type ItemGetRequest struct {
	JSONRPC string      `json:"jsonrpc"`
	Method  string      `json:"method"`
	Params  interface{} `json:"params,omitempty"`
	ID      int         `json:"id"`
}

func main() {
	envFile := "env.txt"

	// Değerleri env.txt dosyasından oku
	url, method, authorization, contentType, err := readEnvFile(envFile)
	if err != nil {
		fmt.Println(err)
		return
	}

	itemParams, err := getItemParamsFromArguments(os.Args)
	if err != nil {
		fmt.Println(err)
		return
	}

	itemGetRequest := ItemGetRequest{
		JSONRPC: "2.0",
		Method:  "item.get",
		Params:  itemParams,
		ID:      1,
	}

	payload := strings.NewReader(itemGetRequest.String())

	client := &http.Client{}
	req, err := http.NewRequest(method, url, payload)
	if err != nil {
		fmt.Println(err)
		return
	}
	req.Header.Add("Authorization", authorization)
	req.Header.Add("Content-Type", contentType)

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

func readEnvFile(file string) (string, string, string, string, error) {
	envData, err := ioutil.ReadFile(file)
	if err != nil {
		return "", "", "", "", err
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

	return url, method, authorization, contentType, nil
}

func getItemParamsFromArguments(args []string) (interface{}, error) {
	var hostIDs []string
	var output []string
	var itemIDs []string

	for _, arg := range args[1:] {
		parts := strings.SplitN(arg, ":", 2)
		if len(parts) != 2 {
			return nil, fmt.Errorf("Geçersiz argüman formatı: %s", arg)
		}

		key := strings.TrimSpace(parts[0])
		value := strings.TrimSpace(parts[1])

		switch key {
		case "hostid":
			hostIDs = strings.Split(value, ",")
		case "output":
			output = strings.Split(value, ",")
		case "itemids":
			itemIDs = strings.Split(value, ",")
		default:
			return nil, fmt.Errorf("Bilinmeyen argüman: %s", key)
		}
	}

	itemParams := make(map[string]interface{})
	if len(hostIDs) > 0 {
		itemParams["hostids"] = hostIDs
	}
	if len(output) > 0 {
		itemParams["output"] = output
	}
	if len(itemIDs) > 0 {
		itemParams["itemids"] = itemIDs
	}

	return itemParams, nil
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
