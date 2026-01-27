# ========== R CODE SAMPLES ==========

# ---------- FILE 1: basics.R ----------
# Basic R operations and functions

# Set working directory
setwd("~/R_projects")

# Create some vectors
numbers <- c(23, 45, 67, 89, 12, 34, 56, 78, 90)
names <- c("Alice", "Bob", "Charlie", "David", "Eve")
temperatures <- c(72.5, 68.3, 75.1, 69.8, 71.2, 73.4)

# Basic statistics
cat("Basic Statistics:\n")
cat("Mean temperature:", mean(temperatures), "\n")
cat("Median temperature:", median(temperatures), "\n")
cat("Standard deviation:", sd(temperatures), "\n")
cat("Range:", range(temperatures), "\n\n")

# Create a simple function
fahrenheit_to_celsius <- function(f) {
  return((f - 32) * 5/9)
}

cat("Temperature conversions:\n")
for(temp in temperatures) {
  celsius <- fahrenheit_to_celsius(temp)
  cat(sprintf("%.1f°F = %.1f°C\n", temp, celsius))
}

# ---------- FILE 2: data_frames.R ----------
# Working with data frames

# Create a data frame
students <- data.frame(
  id = 1:5,
  name = c("John", "Sarah", "Mike", "Lisa", "Tom"),
  age = c(20, 22, 21, 23, 20),
  grade = c(85, 92, 78, 95, 88),
  major = c("CS", "Bio", "Math", "CS", "Econ")
)

print("Students data frame:")
print(students)
cat("\n")

# Summary statistics
cat("Summary of students:\n")
print(summary(students))

# Filter data
cs_students <- students[students$major == "CS", ]
cat("\nComputer Science students:\n")
print(cs_students)

# Add a new column
students$status <- ifelse(students$grade >= 90, "Excellent", 
                         ifelse(students$grade >= 80, "Good", "Average"))

# ---------- FILE 3: plotting.R ----------
# Data visualization

# Create sample data
months <- c("Jan", "Feb", "Mar", "Apr", "May", "Jun")
sales <- c(120, 135, 148, 165, 190, 205)
expenses <- c(85, 90, 95, 105, 115, 125)
profit <- sales - expenses

# Bar plot
barplot(sales, names.arg = months, 
        main = "Monthly Sales", 
        col = "skyblue",
        ylab = "Sales ($)",
        xlab = "Month")

# Line plot
plot(sales, type = "b", col = "blue", lwd = 2,
     main = "Sales Trend", 
     ylab = "Amount ($)",
     xlab = "Month",
     ylim = c(0, max(sales) * 1.1))
lines(expenses, type = "b", col = "red", lwd = 2)
lines(profit, type = "b", col = "green", lwd = 2)
legend("topleft", 
       legend = c("Sales", "Expenses", "Profit"),
       col = c("blue", "red", "green"),
       lty = 1, lwd = 2)

# Scatter plot
set.seed(123)  # For reproducibility
x <- rnorm(100, mean = 50, sd = 10)
y <- 2*x + rnorm(100, mean = 0, sd = 15)
plot(x, y, 
     main = "Scatter Plot with Regression Line",
     xlab = "X Variable",
     ylab = "Y Variable",
     pch = 19, col = "purple")
abline(lm(y ~ x), col = "red", lwd = 2)

# ---------- FILE 4: functions.R ----------
# Custom functions and control structures

# Function to calculate Fibonacci sequence
fibonacci <- function(n) {
  if (n <= 0) return(numeric(0))
  if (n == 1) return(0)
  if (n == 2) return(c(0, 1))
  
  fib <- numeric(n)
  fib[1] <- 0
  fib[2] <- 1
  
  for (i in 3:n) {
    fib[i] <- fib[i-1] + fib[i-2]
  }
  
  return(fib)
}

cat("Fibonacci sequence (first 10 numbers):\n")
print(fibonacci(10))

# Function to check if a number is prime
is_prime <- function(num) {
  if (num <= 1) return(FALSE)
  if (num == 2) return(TRUE)
  if (num %% 2 == 0) return(FALSE)
  
  for (i in 3:sqrt(num)) {
    if (num %% i == 0) return(FALSE)
  }
  return(TRUE)
}

cat("\nPrime numbers between 1 and 30:\n")
primes <- numeric(0)
for (i in 1:30) {
  if (is_prime(i)) {
    primes <- c(primes, i)
  }
}
print(primes)

# ---------- FILE 5: file_operations.R ----------
# Reading and writing files

# Create sample data to write
weather_data <- data.frame(
  date = seq(as.Date("2024-01-01"), by = "day", length.out = 7),
  temp_high = c(72, 75, 68, 80, 77, 73, 70),
  temp_low = c(55, 58, 52, 60, 57, 54, 53),
  precipitation = c(0.1, 0, 0.5, 0, 0.2, 0, 0)
)

# Write to CSV
write.csv(weather_data, "weather_data.csv", row.names = FALSE)
cat("Weather data written to 'weather_data.csv'\n")

# Read from CSV
read_data <- read.csv("weather_data.csv")
cat("\nFirst few rows of read data:\n")
print(head(read_data))

# Basic analysis
cat("\nWeather Summary:\n")
cat("Average high temperature:", mean(read_data$temp_high), "°F\n")
cat("Average low temperature:", mean(read_data$temp_low), "°F\n")
cat("Total precipitation:", sum(read_data$precipitation), "inches\n")

# ---------- FILE 6: packages.R ----------
# Using R packages (if installed)

# Check and install packages if needed
required_packages <- c("dplyr", "ggplot2")

for (pkg in required_packages) {
  if (!require(pkg, character.only = TRUE)) {
    cat(pkg, "not installed. Installing...\n")
    install.packages(pkg)
    library(pkg, character.only = TRUE)
  } else {
    cat(pkg, "is already installed\n")
  }
}

# Example with dplyr
library(dplyr)

# Create a sample dataframe
product_data <- data.frame(
  product_id = 1:10,
  category = sample(c("Electronics", "Clothing", "Books", "Home"), 10, replace = TRUE),
  price = round(runif(10, 10, 500), 2),
  quantity = sample(1:100, 10)
)

cat("\nProduct data:\n")
print(product_data)

# Use dplyr for data manipulation
summary_stats <- product_data %>%
  group_by(category) %>%
  summarize(
    avg_price = mean(price),
    total_quantity = sum(quantity),
    max_price = max(price),
    min_price = min(price)
  )

cat("\nSummary by category:\n")
print(summary_stats)

# ---------- FILE 7: lists_matrices.R ----------
# Working with lists and matrices

# Create a list
employee <- list(
  name = "John Doe",
  age = 35,
  department = "IT",
  skills = c("Python", "R", "SQL", "Java"),
  salary = 85000,
  projects = list(
    current = "Data Migration",
    completed = c("CRM Implementation", "Website Redesign")
  )
)

cat("Employee information:\n")
print(employee)

# Access list elements
cat("\nEmployee skills:\n")
for(skill in employee$skills) {
  cat("- ", skill, "\n")
}

# Create a matrix
matrix_data <- matrix(1:12, nrow = 3, ncol = 4)
cat("\nMatrix:\n")
print(matrix_data)

cat("\nMatrix operations:\n")
cat("Transpose:\n")
print(t(matrix_data))

cat("\nMatrix multiplication (with itself transposed):\n")
print(matrix_data %*% t(matrix_data))

# ---------- FILE 8: random_generation.R ----------
# Random data generation and simulation

set.seed(42)  # Set seed for reproducibility

# Generate random data
normal_data <- rnorm(1000, mean = 100, sd = 15)
uniform_data <- runif(500, min = 0, max = 1)
poisson_data <- rpois(200, lambda = 3)

cat("Random Data Generation:\n")
cat("Normal distribution (mean=100, sd=15):\n")
cat("  Sample mean:", mean(normal_data), "\n")
cat("  Sample sd:", sd(normal_data), "\n\n")

cat("Uniform distribution (0 to 1):\n")
hist(uniform_data, main = "Uniform Distribution", col = "lightblue")

cat("Poisson distribution (lambda=3):\n")
table_data <- table(poisson_data)
print(table_data)

# Simulation: Coin flips
n_flips <- 1000
coin_flips <- sample(c("Heads", "Tails"), n_flips, replace = TRUE)
cat("\nCoin flip simulation (", n_flips, " flips):\n", sep = "")
cat("Heads:", sum(coin_flips == "Heads"), "\n")
cat("Tails:", sum(coin_flips == "Tails"), "\n")
cat("Proportion of heads:", mean(coin_flips == "Heads"), "\n")

# ---------- SCRIPT END ----------
cat("\n=== All scripts completed successfully! ===\n")
