package main

import (
	"bufio"
	"fmt"
	"os"
	"slices"
)

const (
	Black = 1
	Blue  = 2
)

type EdgeDesc struct {
	fromVertex int
	toVertex   int
}

type VertexDesc struct {
	vertex     int
	k          int
	l          int
	isCutPoint bool
	childCnt   int
}

type GrahpDesc struct {
	adjacencyMap map[int][]int
	vertexMap    map[int]*VertexDesc
	edgeMap      map[EdgeDesc]int
	edgeCnt      int
	vertexCnt    int
}

func createEdgeDesc(vertexLeft, vertexRight int) EdgeDesc {
	return EdgeDesc{fromVertex: min(vertexLeft, vertexRight), toVertex: max(vertexLeft, vertexRight)}
}

func readGraphInfo() (GrahpDesc, error) {

	var vertexLeft int
	var vertexRight int

	adjacencyList := make(map[int][]int, 0)
	edgeMap := make(map[EdgeDesc]int, 0)
	vertexMap := make(map[int]*VertexDesc, 0)

	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		_, _ = fmt.Sscanf(scanner.Text(), "%d %d", &vertexLeft, &vertexRight)

		edge := EdgeDesc{fromVertex: min(vertexLeft, vertexRight), toVertex: max(vertexLeft, vertexRight)}
		edgeMap[edge]++

		vertexLeftDesc := VertexDesc{vertex: vertexLeft, k: 1, l: 1, isCutPoint: false, childCnt: 0}
		vertexRightDesc := VertexDesc{vertex: vertexRight, k: 1, l: 1, isCutPoint: false, childCnt: 0}

		vertexMap[vertexLeft] = &vertexLeftDesc
		vertexMap[vertexRight] = &vertexRightDesc

		_, ok := adjacencyList[vertexLeft]
		if !ok {
			adjacencyList[vertexLeft] = make([]int, 0)
		}
		_, ok = adjacencyList[vertexRight]
		if !ok {
			adjacencyList[vertexRight] = make([]int, 0)
		}
		adjacencyList[vertexLeft] = append(adjacencyList[vertexLeft], vertexRight)
		adjacencyList[vertexRight] = append(adjacencyList[vertexRight], vertexLeft)
	}

	return GrahpDesc{adjacencyList, vertexMap, edgeMap, len(edgeMap), len(vertexMap)}, nil
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

func printMap(adjacencyMap map[int][]int) {
	for key, slice := range adjacencyMap {
		fmt.Print(key, ": ")
		for i := range slice {
			fmt.Print(slice[i], " ")
		}
		fmt.Print("\n")
	}
}

func printEdgeMap(mp map[EdgeDesc]int) {
	for key, val := range mp {
		fmt.Println("key: ", key, "val: ", val)
	}
}

func printVertexMap(vertexMap map[int]*VertexDesc) {
	for key, val := range vertexMap {
		fmt.Println("key: ", key, "val: ", val)
	}
}

func printVisited(v []int) {
	for i := range v {
		fmt.Print(v[i], " ")
	}
	fmt.Print("\n")
}

func (graph *GrahpDesc) checkRoot(rootVertex int) bool {

	adjacencyMap := graph.adjacencyMap
	edgeMap := graph.edgeMap

	treeAdgesCnt := 0
	for _, adjVertex := range adjacencyMap[rootVertex] {
		if edgeMap[createEdgeDesc(rootVertex, adjVertex)] == Blue {
			treeAdgesCnt++
		}
	}
	return treeAdgesCnt >= 2
}

func (graph *GrahpDesc) getMinKAncestor(curVertex int) int {

	adjacencyMap := graph.adjacencyMap
	vertexMap := graph.vertexMap
	edgeMap := graph.edgeMap

	minKAncestor := 1<<63 - 1

	curVertexDesc := vertexMap[curVertex]
	for _, adjVertex := range adjacencyMap[curVertex] {

		adjVertexDesc := vertexMap[adjVertex]
		if adjVertexDesc.k < curVertexDesc.k && edgeMap[createEdgeDesc(curVertex, adjVertex)] == Black {
			//fmt.Println("vertex: ", curVertex, "vertex k ", curVertexDesc.k, "parent vertex", adjVertex, "parent vertex k ", adjVertexDesc.k)
			if adjVertexDesc.k < minKAncestor {
				minKAncestor = adjVertexDesc.k
			}
		}
	}
	return minKAncestor
}

func (graph *GrahpDesc) getMinMaxLChild(curVertex int) (int, int) {

	adjacencyMap := graph.adjacencyMap
	vertexMap := graph.vertexMap
	edgeMap := graph.edgeMap

	minLChild := 1<<63 - 1
	maxLChild := 0

	curVertexDesc := vertexMap[curVertex]
	for _, adjVertex := range adjacencyMap[curVertex] {

		adjVertexDesc := vertexMap[adjVertex]

		if adjVertexDesc.k > curVertexDesc.k && edgeMap[createEdgeDesc(curVertex, adjVertex)] == Blue {
			if adjVertexDesc.l < minLChild {
				minLChild = adjVertexDesc.l
			}
			if adjVertexDesc.l > maxLChild {
				maxLChild = adjVertexDesc.l
			}
		}
	}
	return minLChild, maxLChild
}

func (graph *GrahpDesc) paintEdge(vertexLeft, vertexRight int, color int) {
	edgeMap := graph.edgeMap
	edge := createEdgeDesc(vertexLeft, vertexRight)
	edgeMap[edge] = color
}

func (graph *GrahpDesc) buildSpanningTree(curVertex int, visited []bool, k int) {

	adjacencyMap := graph.adjacencyMap
	vertexMap := graph.vertexMap

	visited[curVertex] = true
	curVertexDesc := vertexMap[curVertex]
	curVertexDesc.k = k
	for _, adjVertex := range adjacencyMap[curVertex] {
		if !visited[adjVertex] {
			graph.paintEdge(curVertex, adjVertex, Blue)
			graph.buildSpanningTree(adjVertex, visited, k+1)
		}
	}
	minLChild, maxLChild := graph.getMinMaxLChild(curVertex)
	curVertexDesc.l = min(curVertexDesc.k, graph.getMinKAncestor(curVertex), minLChild)
	if curVertexDesc.k <= maxLChild {
		curVertexDesc.isCutPoint = true
	}
}

func (graph *GrahpDesc) getCutPoints() []int {
	vertexMap := graph.vertexMap
	res := make([]int, 0)
	for i := range vertexMap {
		if vertexMap[i].isCutPoint {
			res = append(res, vertexMap[i].vertex)
		}
	}
	slices.Sort(res)
	return res
}

func printCutPoints(cutPoints []int) {

	for _, val := range cutPoints {
		fmt.Print(val, " ")
	}
}

func fincCutPointsHopcroftTarjan(graph GrahpDesc) {
	root := 0
	visited := make([]bool, graph.vertexCnt)
	graph.buildSpanningTree(root, visited, 1)
	graph.vertexMap[root].isCutPoint = graph.checkRoot(root)
	printCutPoints(graph.getCutPoints())
}

func main() {
	graph, _ := readGraphInfo()
	fincCutPointsHopcroftTarjan(graph)
}
