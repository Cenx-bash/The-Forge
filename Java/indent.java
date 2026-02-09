// Queue.java
public class Queue {
    private int maxSize;
    private int[] queueArray;
    private int front;
    private int rear;
    private int currentSize;
    
    public Queue(int size) {
        maxSize = size;
        queueArray = new int[maxSize];
        front = 0;
        rear = -1;
        currentSize = 0;
    }
    
    // Enqueue operation
    public void enqueue(int value) {
        if (isFull()) {
            System.out.println("Queue is full!");
            return;
        }
        rear = (rear + 1) % maxSize;
        queueArray[rear] = value;
        currentSize++;
    }
    
    // Dequeue operation
    public int dequeue() {
        if (isEmpty()) {
            System.out.println("Queue is empty!");
            return -1;
        }
        int value = queueArray[front];
        front = (front + 1) % maxSize;
        currentSize--;
        return value;
    }
    
    // Peek operation
    public int peek() {
        if (isEmpty()) {
            System.out.println("Queue is empty!");
            return -1;
        }
        return queueArray[front];
    }
    
    // Check if queue is empty
    public boolean isEmpty() {
        return currentSize == 0;
    }
    
    // Check if queue is full
    public boolean isFull() {
        return currentSize == maxSize;
    }
    
    // Get queue size
    public int size() {
        return currentSize;
    }
}
