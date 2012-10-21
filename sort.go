package main

import (
	"fmt"
	"sort"
)

type SortBy struct {
	Data []string
	IsLess func (l, r string) bool
}

func (sb SortBy) Less(i, j int) bool {
	return sb.IsLess(sb.Data[i], sb.Data[j])
}

func (sb SortBy) Swap(i, j int) {
	sb.Data[i], sb.Data[j] = sb.Data[j], sb.Data[i]
}

func (sb SortBy) Len() int {
	return len(sb.Data)
}

func main() {
	s := []string{"zzz", "a12345", "b", "cc", "d"};
	sort.Sort(SortBy{s, func (l, r string) bool {return l<r}})
	fmt.Println(s)
	sort.Sort(SortBy{s, func (l, r string) bool {return len(l)<len(r)}})
	fmt.Println(s)
}
