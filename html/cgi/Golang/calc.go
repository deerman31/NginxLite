package main

import (
	"fmt"
	"os"
	"strings"
	"net/http"
	"net/http/cgi"
	"strconv"
)

func split(s, d string) []string {
	var list []string
	var a string

	for _, char := range s {
		if strings.ContainsRune(d, char) {
			if a != "" {
				list = append(list, a)
			}
			list = append(list, string(char))
			a = ""
		} else {
			a += string(char)
		}
	}
	if a != "" {
		list = append(list, a)
	}
	return list
}

func isDigit(s string) bool {
	for _, c := range s {
		if c < '0' || c > '9' {
			return false
		}
	}
	return true
}

func isMark(s string) bool {
	if len(s) != 1 {
		return false
	}
	if s == "+" {
		return true
	}
	if s == "-" {
		return true
	}
	if s == "*" {
		return true
	}
	if s == "/" {
		return true
	}
	return false
}

func isCalc(list []string) bool {
	if len(list)%2 == 0 {
		return false
	}

	for i := 0; i < len(list); i += 1 {
		if i%2 == 0 {
			if isDigit(list[i]) == false {
				return false
			}
		} else {
			if isMark(list[i]) == false {
				return false
			}
		}
	}
	return true
}

func Calc(l []string) string {
	var v, n int
	var m string

	for i := 0; i < len(l); i += 1 {
		if i%2==0 {
			if i == 0 {
				v, _ = strconv.Atoi(l[i])
			} else {
				n, _ = strconv.Atoi(l[i])
			}
			if m == "+" {
				v += n
			} else if m == "-" {
				v -= n
			} else if m == "*" {
				v *= n
			} else if m == "/" {
				v /= n
			}
		} else {
			m = l[i]
		}
	}
	return strconv.Itoa(v)
}

func main() {
	handler := http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		var s, v string
		w.Header().Set("Content-Type", "text/plain; charset=UTF-8")
		query := os.Getenv("QUERY_STRING")
		if query == "" {
			s = "QUERY_STRINGがセットされていない"
		} else {
			list := split(query, "+-*/")
			if isCalc(list) == false {
				s = "書式が間違っており、計算できません"
				//s += "\n" + query
			} else {
				s = query
				v = Calc(list)
			}
		}
		fmt.Fprintln(w, s)
		if (len(v) != 0) {
			fmt.Fprintln(w, v)
		}
	})

	if err := cgi.Serve(handler); err != nil {
		fmt.Println(err)
	}
}
