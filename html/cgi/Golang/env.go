package main

import (
	"fmt"
	"net/http"
	"os"
	"net/http/cgi"
	//"strconv"
)

func main() {
	handler := http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "text/plain")
		var s string
		for _, env := range os.Environ() {
			s += env + "\r\n"
		}
		//w.Header().Set("Content-Length", strconv.Itoa(len(s)))
		fmt.Fprintln(w, s)
	})

	if err := cgi.Serve(handler); err != nil {
		fmt.Println(err)
	}
}
