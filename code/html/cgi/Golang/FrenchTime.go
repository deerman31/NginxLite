package main

import (
	"fmt"
	//"net/http"
	"os"
	"time"
)

func main() {
	// CGIとして実行されるためのHTTPヘッダーを設定
	fmt.Println("Content-Type: text/html; charset=UTF-8")
	fmt.Println()

	// フランスのタイムゾーンを取得（パリを例としています）
	loc, err := time.LoadLocation("Europe/Paris")
	if err != nil {
		fmt.Fprintf(os.Stderr, "タイムゾーンの読み込みに失敗しました: %s", err)
		os.Exit(1)
	}

	// 現在の時刻をフランスのタイムゾーンで取得
	now := time.Now().In(loc)

	// HTML形式で時刻を出力
	fmt.Println("<!DOCTYPE html>")
	fmt.Println("<html>")
	fmt.Println("<head>")
	fmt.Println("<title>現在のフランスの時刻</title>")
	fmt.Println("</head>")
	fmt.Println("<body>")
	fmt.Printf("<h1>フランスの現在時刻: %s</h1>", now.Format("2006-01-02 15:04:05"))
	fmt.Println("</body>")
	fmt.Println("</html>")
}
