// Stack.java
public class Stack {
    private int maxSize;
    private int[] stackArray;
    private int top;
    
    public Stack(int size) {
        maxSize = size;
        stackArray = new int[maxSize];
        top = -1;
    }
    
    // Push operation
    public void push(int value) {
        if (isFull()) {
            System.out.println("Stack is full!");
            return;
        }
        stackArray[++top] = value;
    }
    
    // Pop operation
    public int pop() {
        if (isEmpty()) {
            System.out.println("Stack is empty!");
            return -1;
        }
        return stackArray[top--];
    }
    
    // Peek operation
    public int peek() {
        if (isEmpty()) {
            System.out.println("Stack is empty!");
            return -1;
        }
        return stackArray[top];
    }
    
    // Check if stack is empty
    public boolean isEmpty() {
        return top == -1;
    }
    
    // Check if stack is full
    public boolean isFull() {
        return top == maxSize - 1;
    }
    
    // Get stack size
    public int size() {
        return top + 1;
    }
}
