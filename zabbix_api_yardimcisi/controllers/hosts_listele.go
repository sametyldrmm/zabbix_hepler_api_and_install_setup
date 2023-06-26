package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"strings"
)

type HostResult struct {
	HostID    string `json:"hostid"`
	Host      string `json:"host"`
	Status    string `json:"status"`
	Name      string `json:"name"`
	Inventory struct {
		OS string `json:"os"`
	} `json:"inventory"`
}

type APIResponse struct {
	JSONRPC string       `json:"jsonrpc"`
	Result  []HostResult `json:"result"`
	ID      int          `json:"id"`
}

func main() {
	envFile := "env.txt"

	// Değerleri env.txt dosyasından oku
	url, method, authorization, contentType, err := readEnvFile(envFile)
	if err != nil {
		fmt.Println(err)
		return
	}

	payload := strings.NewReader(`{
		"jsonrpc": "2.0",
		"method": "host.get",
		"params": {
			"output": ["hostid","host","status","name"],
			"selectInventory": ["os"]
		},
		"id": 1
	}`)

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

	var response APIResponse
	err = json.Unmarshal(body, &response)
	if err != nil {
		fmt.Println(err)
		return
	}

	fmt.Printf("%-10s %-20s %-10s %-20s %-20s\n", "Host ID", "Host", "Status", "Name", "Operating System")
	for _, host := range response.Result {
		fmt.Printf("%-10s %-20s %-10s %-20s %-20s\n", host.HostID, host.Host, host.Status, host.Name, host.Inventory.OS)
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
