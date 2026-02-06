// DSAExample.java
public class DSAExample {
    public static void main(String[] args) {
        System.out.println("=== Linked List Example ===");
        LinkedList list = new LinkedList();
        list.append(10);
        list.append(20);
        list.prepend(5);
        list.printList();
        
        System.out.println("\n=== Binary Search Tree Example ===");
        BinarySearchTree bst = new BinarySearchTree();
        bst.insert(50);
        bst.insert(30);
        bst.insert(70);
        bst.insert(20);
        System.out.print("In-order traversal: ");
        bst.inorder();
        
        System.out.println("\n=== Sorting Algorithms Example ===");
        SortingAlgorithms sorting = new SortingAlgorithms();
        int[] arr = {64, 34, 25, 12, 22, 11, 90};
        System.out.print("Original array: ");
        sorting.printArray(arr);
        
        int[] arrForQuick = arr.clone();
        sorting.quickSort(arrForQuick, 0, arrForQuick.length - 1);
        System.out.print("Quick sorted: ");
        sorting.printArray(arrForQuick);
        
        System.out.println("\n=== Searching Algorithms Example ===");
        SearchingAlgorithms searching = new SearchingAlgorithms();
        int[] sortedArr = {2, 5, 8, 12, 16, 23, 38, 56, 72, 91};
        int target = 23;
        int result = searching.binarySearch(sortedArr, target);
        System.out.println("Binary search for " + target + ": found at index " + result);
        
        System.out.println("\n=== Stack Example ===");
        Stack stack = new Stack(5);
        stack.push(10);
        stack.push(20);
        stack.push(30);
        System.out.println("Popped: " + stack.pop());
        System.out.println("Top element: " + stack.peek());
        
        System.out.println("\n=== Queue Example ===");
        Queue queue = new Queue(5);
        queue.enqueue(10);
        queue.enqueue(20);
        queue.enqueue(30);
        System.out.println("Dequeued: " + queue.dequeue());
        System.out.println("Front element: " + queue.peek());
    }
}
