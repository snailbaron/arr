package xs

import (
	"cmp"
	"iter"
	"maps"
	"slices"
)

func InsertAt[S ~[]E, E any](s S, i int, value E) []E {
	if i >= cap(s) {
		s = append(s[:cap(s)], make([]E, i+1-cap(s))...)[:i+1]
	} else if i >= len(s) {
		s = s[:i+1]
	}
	s[i] = value
	return s
}

func InOrder[Map ~map[K]V, K cmp.Ordered, V any](m Map) iter.Seq2[K, V] {
	return func(yield func(K, V) bool) {
		for _, k := range slices.Sorted(maps.Keys(m)) {
			if !yield(k, m[k]) {
				return
			}
		}
	}
}

func ElementSet[E comparable](s []E) map[E]int {
	m := make(map[E]int)
	for _, e := range s {
		m[e]++
	}
	return m
}

func SameElements[E comparable](s1, s2 []E) bool {
	m1 := ElementSet(s1)
	m2 := ElementSet(s2)
	return maps.Equal(m1, m2)
}
