# Graph-based-Book-Recommendation


Welcome to the **Book Recommendation System**! This project is a personalized book recommendation engine built using the **C programming language**. It’s designed to help users manage a collection of books and get customized recommendations based on their preferences. The project is **free to use** and open for contributions.

## Features

### 1. Book Management
- Load book data from a CSV file, including details like title, author, genre, rating, and popularity.
- View all available books and their information.

### 2. User Management
- Add users to the system and manage their preferred books.
- Efficiently handle user data using a **hash table**.

### 3. Graph Representation
- Represent relationships between books using a **graph data structure**.
- Connect books based on shared authors or genres for sophisticated recommendations.

### 4. Recommendation Engine
- Get recommendations tailored to user preferences and selected genres.
- Choose between **popular** or **underrated** book suggestions.

### 5. User Interface
- Interact with the system via a **simple text-based menu**.
- Options to view books, add users, and receive recommendations.

## Data Structures
- **Book**: Stores details such as ID, title, author, genre, rating, and popularity.
- **User**: Maintains user details, including ID, name, and preferred books.
- **Graph**: Represents relationships between books using adjacency lists.

## How It Works
1. **Run the Program**: Start the program to access the menu-driven interface.
2. **Load Books**: Import book data from a CSV file.
3. **Manage Users**: Add new users and update their preferences.
4. **Get Recommendations**: Receive personalized book suggestions based on preferences.

## Getting Started

### Prerequisites
- A C compiler (e.g., GCC).
- A CSV file containing book data in the following format:
  ```csv
  ID,Title,Author,Genre,Rating,Popularity
  1,Book Title,Author Name,Genre,4.5,High
  ```

### Installation
1. Clone this repository:
   ```bash
   git clone https://github.com/your-username/book-recommendation-system.git
   ```
2. Navigate to the project directory:
   ```bash
   cd book-recommendation-system
   ```
3. Compile the program:
   ```bash
   gcc main.c -o book_rec_system
   ```
4. Run the program:
   ```bash
   ./book_rec_system
   ```

## Contributing
We welcome contributions to improve the **Book Recommendation System**! Here’s how you can get started:

1. Fork the repository.
2. Create a new branch:
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. Make your changes and commit them:
   ```bash
   git commit -m "Add your feature description"
   ```
4. Push to your branch:
   ```bash
   git push origin feature/your-feature-name
   ```
5. Open a Pull Request and describe your changes.

## License
This project is licensed under the MIT License. Feel free to use, modify, and distribute it as you like.

## Feedback
If you encounter any issues or have suggestions for improvements, feel free to open an issue or contact us directly. We’d love to hear from you!

---

### Let’s build something amazing together! 🌟

