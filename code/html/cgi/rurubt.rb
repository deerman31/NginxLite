#!/usr/bin/env ruby
require 'cgi'
cgi = CGI.new

# ヘッダーを出力
puts cgi.header

# HTML開始
puts "<html>"
puts "<head>"
puts "<title>Prime Numbers</title>"
puts "</head>"
puts "<body>"

# 素数を計算する関数
def prime_numbers(limit)
  primes = []
  for i in 2..limit
    prime_flag = true
    for j in 2..Math.sqrt(i)
      if i % j == 0
        prime_flag = false
        break
      end
    end
    primes << i if prime_flag
  end
  primes
end

# フォームデータを取得（上限値）
limit = cgi['limit'].to_i
limit = 100 if limit <= 0  # デフォルト値

# 素数を計算
primes = prime_numbers(limit)

# 素数を表示
puts "<h1>Prime Numbers up to #{limit}</h1>"
puts "<ul>"
primes.each do |prime|
  puts "<li>#{prime}</li>"
end
puts "</ul>"

# HTML終了
puts "</body>"
puts "</html>"
hey ("hello,world")
