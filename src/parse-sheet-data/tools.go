package main

import (
	"unicode"
)

func kebabToCamel(s string) string {
	result := ""

	convert := false
	for _, c := range s {
		if c == '-' {
			convert = true
			continue
		}

		if convert {
			convert = false
			c = unicode.ToUpper(c)
		}
		result += string(c)
	}

	return result
}
