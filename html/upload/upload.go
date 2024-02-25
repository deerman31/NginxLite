package main

import (
	"fmt"
	"io/ioutil"
	"net/http"
	"net/http/cgi"
	"os"
)

func uploadFile(w http.ResponseWriter, r *http.Request) {
	// リクエストがPOSTメソッドかどうかを確認
	if r.Method == "POST" {
		// フォームのファイルデータを解析
		err := r.ParseMultipartForm(10 << 20) // 10 MBのファイルサイズ上限
		if err != nil {
			http.Error(w, err.Error(), http.StatusBadRequest)
			return
		}

		// フォームからファイルを取得
		file, handler, err := r.FormFile("uploadFile")
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}
		defer file.Close()

		// アップロードされたファイルを保存するパス（サーバー上で適切な権限を設定する必要があります）
		uploadPath := "dir_upload/"

		// アップロードされたファイルの内容を読み取る
		fileBytes, err := ioutil.ReadAll(file)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}

		// ファイルを保存
		err = ioutil.WriteFile(uploadPath+handler.Filename, fileBytes, 0666)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}

		// 成功メッセージを表示
		fmt.Fprintf(w, "ファイルアップロードに成功しました: %s", handler.Filename)
	} else {
		// POSTメソッドでないリクエストにはエラーを返す
		http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
	}
}

func main() {
	http.HandleFunc("/", uploadFile)

	// CGIサーバーとして起動
	if err := cgi.Serve(nil); err != nil {
		fmt.Fprintf(os.Stderr, "cgi.Serve: %v\n", err)
		os.Exit(1)
	}
}
