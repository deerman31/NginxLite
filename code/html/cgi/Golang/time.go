package main

import (
	"fmt"
	"time"
)

func main() {
	jst := time.FixedZone("Asia/Tokyo", 9*60*60)
	jstTime := time.Now().In(jst)

	// HTML本文の生成
	htmlContent := "<!DOCTYPE html>\r\n" +
		"<html>\r\n" +
		"<head>\r\n" +
		"<title>Complete HTTP Response with Content-Length</title>\r\n" +
		"</head>\r\n" +
		"<body>\r\n" +
		fmt.Sprintf("<h1>Hello, World!</h1>\r\n") +
		fmt.Sprintf("<p>Current Japan Time: %s</p>\r\n", jstTime.Format("2006-01-02 15:04:05")) +
		"<p>This is a complete HTTP response from a CGI script, including Content-Length.</p>\r\n" +
		"</body>\r\n" +
		"</html>\r\n"

	// Content-Lengthの計算
	//contentLength := len([]byte(htmlContent))

	// HTTPレスポンスヘッダーの出力
	//fmt.Printf("HTTP/1.1 200 OK\r\n")
	fmt.Printf("Content-Type: text/html\r\n")
	//fmt.Printf("Content-Length: %d\r\n", contentLength)
	fmt.Printf("\r\n")
	
	// HTML本文の出力
	fmt.Printf(htmlContent)
}
