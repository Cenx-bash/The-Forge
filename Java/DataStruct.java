// BinarySearchTree.java
public class BinarySearchTree {
    private Node root;
    
    private class Node {
        int data;
        Node left, right;
        
        Node(int data) {
            this.data = data;
            this.left = this.right = null;
        }
    }
    
    // Insert a node
    public void insert(int data) {
        root = insertRec(root, data);
    }
    
    private Node insertRec(Node root, int data) {
        if (root == null) {
            root = new Node(data);
            return root;
        }
        
        if (data < root.data) {
            root.left = insertRec(root.left, data);
        } else if (data > root.data) {
            root.right = insertRec(root.right, data);
        }
        return root;
    }
    
    // Search for a node
    public boolean search(int data) {
        return searchRec(root, data);
    }
    
    private boolean searchRec(Node root, int data) {
        if (root == null) return false;
        if (root.data == data) return true;
        
        if (data < root.data) {
            return searchRec(root.left, data);
        } else {
            return searchRec(root.right, data);
        }
    }
    
    // In-order traversal
    public void inorder() {
        inorderRec(root);
        System.out.println();
    }
    
    private void inorderRec(Node root) {
        if (root != null) {
            inorderRec(root.left);
            System.out.print(root.data + " ");
            inorderRec(root.right);
        }
    }
    
    // Find minimum value
    public int findMin() {
        return findMinRec(root);
    }
    
    private int findMinRec(Node root) {
        if (root.left == null) {
            return root.data;
        }
        return findMinRec(root.left);
    }
}
