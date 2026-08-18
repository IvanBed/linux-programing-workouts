package main

import "fmt"

type Edge struct {
	fromVertex int
	toVertex   int
}

func readGraphInfo() ([][]int, map[Edge]int, error) {
	var vertexCnt int
	var edgeCnt int

	var vertexLeft int
	var vertexRight int

	_, err := fmt.Scanf("%d", &vertexCnt)

	if err != nil {
		//fmt.Println("Can not read vertex count")
		return nil, nil, err
	}

	_, err = fmt.Scanf("%d", &edgeCnt)

	if err != nil {
		//fmt.Println("Can not read edgecount")
		return nil, nil, err
	}

	adjacencyList := make([][]int, vertexCnt+1)

	edgeMap := make(map[Edge]int, edgeCnt)

	for i := range adjacencyList {
		adjacencyList[i] = make([]int, 0)
	}

	for i := 0; i < edgeCnt; i++ {
		_, err = fmt.Scanf("%d", &vertexLeft)
		_, err = fmt.Scanf("%d", &vertexRight)

		edgeTo := Edge{fromVertex: vertexLeft, toVertex: vertexRight}
		edgeFrom := Edge{fromVertex: vertexRight, toVertex: vertexLeft}
		edgeMap[edgeTo]++
		edgeMap[edgeFrom]++

		adjacencyList[vertexLeft] = append(adjacencyList[vertexLeft], vertexRight)
		adjacencyList[vertexRight] = append(adjacencyList[vertexRight], vertexLeft)
	}

	return adjacencyList, edgeMap, nil
}

func printList(adjacencyList [][]int) {
	for i := range adjacencyList {
		fmt.Print(i, ": ")
		for j := range adjacencyList[i] {
			fmt.Print(adjacencyList[i][j], " ")
		}
		fmt.Print("\n")
	}
}

func printEdgeMap(mp map[Edge]int) {
	for key, val := range mp {
		fmt.Println("key: ", key, "val: ", val)
	}
}

func printVisited(v []int) {
	for i := range v {
		fmt.Print(v[i], " ")
	}
	fmt.Print("\n")
}

func printEulerianСycleVertices(adjacencyList [][]int, vertexDegree []int, visitedEdges map[Edge]int, vertex int, startVertex int) {

	fmt.Print(vertex, " ")
	for i := range adjacencyList[vertex] {
		edgeTo := Edge{fromVertex: vertex, toVertex: adjacencyList[vertex][i]}
		edgeFrom := Edge{fromVertex: adjacencyList[vertex][i], toVertex: vertex}
		if visitedEdges[edgeTo] > 0 && visitedEdges[edgeFrom] > 0 && vertexDegree[adjacencyList[vertex][i]] > 0 {
			vertexDegree[vertex]--
			vertexDegree[adjacencyList[vertex][i]]--
			visitedEdges[edgeTo]--
			visitedEdges[edgeFrom]--

			if !(adjacencyList[vertex][i] == startVertex && vertexDegree[adjacencyList[vertex][i]] < 1) {
				printEulerianСycleVertices(adjacencyList, vertexDegree, visitedEdges, adjacencyList[vertex][i], startVertex)
			}
		}
	}
}

func simpleDfs(adjacencyList [][]int, visited []bool, vertex int) {
	visited[vertex] = true
	for i := range adjacencyList[vertex] {
		if !visited[adjacencyList[vertex][i]] {

			simpleDfs(adjacencyList, visited, adjacencyList[vertex][i])
		}
	}
}

func isConnected(adjacencyList [][]int) bool {

	visited := make([]bool, len(adjacencyList))
	simpleDfs(adjacencyList, visited, 1)
	for i := 1; i < len(visited); i++ {
		if !visited[i] {
			return false
		}
	}
	return true
}

func checkIfEulerianСycleExists(adjacencyList [][]int) (bool, []int) {
	vertexDegree := make([]int, len(adjacencyList))
	res := isConnected(adjacencyList)

	if !res {
		return res, vertexDegree
	}

	for index, vertexList := range adjacencyList {
		if index > 0 {
			vertexDegree[index] = len(vertexList)
		}
		if len(vertexList)%2 != 0 {
			res = res && false
		}
	}

	return res, vertexDegree
}

func main() {
	adjacencyList, visitedEdges, _ := readGraphInfo()

	eulerianСycle, vertexDegree := checkIfEulerianСycleExists(adjacencyList)
	if !eulerianСycle {
		fmt.Println("NONE")
	} else {
		fmt.Println("YES")

		func() {
			for i := 1; i < len(adjacencyList); i++ {
				if vertexDegree[i] > 1 {
					printEulerianСycleVertices(adjacencyList, vertexDegree, visitedEdges, i, i)
				}
			}
			fmt.Print("\n")
		}()
	}

}
