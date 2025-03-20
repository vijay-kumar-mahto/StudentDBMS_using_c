# StudentDBMS 📚👨‍🎓

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Version](https://img.shields.io/badge/version-1.1.0-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows*%20%7C%20macOS*-lightgrey.svg)

> *A modern, intuitive student database management system built with C and GTK4.*

<div align="center">
  <img src="resources/logo.png" alt="StudentDBMS Logo" width="200"/>
  <br>
  <i>Simplifying student records management for educational institutions</i>
</div>

## ✨ Features

- **📝 Student Management** - Add, update, delete, and list student records with ease
- **📊 Data Visualization** - View department-wise student distribution through interactive pie charts
- **🔍 Intuitive UI** - User-friendly interface with sidebar navigation and responsive design
- **📤 Export Functionality** - Export student data to CSV with a single click
- **🔄 Activity Logging** - Real-time activity tracking with persistent logs
- **🌓 Light/Dark Theme** - Toggle between themes for different lighting conditions
- **🔉 Voice Feedback** - Optional voice synthesis for accessibility (requires eSpeak)
- **⌨️ Keyboard Shortcuts** - Enhance productivity with convenient keyboard controls

## 🖼️ Screenshots

<div align="center">
<img src="Resources/thumbnails/menu.png" alt="Main Menu" width="20%">
<img src="Resources/thumbnails/setting.png" alt="Settings Screen" width="20%">
<img src="Resources/thumbnails/level.png" alt="Level Selection" width="20%">
<img src="Resources/thumbnails/gameplay.png" alt="Gameplay" width="20%">
</div>

## 🛠️ Tech Stack

- **Core:** C programming language
- **GUI:** GTK4 (GTK 4.0 or newer)
- **Database:** SQLite3
- **Graphics:** Cairo
- **Voice Synthesis:** eSpeak (optional)
- **Build System:** CMake

## 📋 Requirements

- GCC or compatible C compiler
- GTK4 development libraries (`libgtk-4-dev`)
- SQLite3 development libraries (`libsqlite3-dev`)
- Cairo development libraries (`libcairo2-dev`)
- CMake (3.10 or newer)
- eSpeak development libraries (`libespeak-dev`) - optional for voice feedback

## 🚀 Installation

### Ubuntu/Debian

#### Install dependencies
```sh
sudo apt update
sudo apt install build-essential cmake libgtk-4-dev libsqlite3-dev libcairo2-dev libespeak-dev
```

#### Clone the repository
```sh
git clone https://github.com/yourusername/StudentDBMS.git
cd StudentDBMS
```

#### Build the project
```sh
mkdir -p cmake-build-debug
cd cmake-build-debug
cmake ..
make
```

#### Run the application
```sh
./StudentDBMS
```

### Other platforms

Support for Windows and macOS is planned. Stay tuned for updates!

## 🎮 Usage

### Main Interface

The application features a sidebar with the following options:

- **Add Student** - Open the form to add a new student record
- **List Students** - View all students in a tabular format
- **Toggle Theme** - Switch between light and dark themes
- **Export to CSV** - Export all student data to CSV format
- **Delete All** - Remove all student records (with confirmation)

### Keyboard Shortcuts

- `Ctrl+S` - Save/Add student
- `Ctrl+M` - Toggle maximize window
- More shortcuts coming soon!

### Student Management

1. **Adding a student:**
    - Click on "Add Student" in the sidebar
    - Fill in the registration number, name, department, email, and mobile number
    - Click "Save" or press `Ctrl+S`

2. **Editing a student:**
    - Go to "List Students" view
    - Double-click on a student record to edit
    - Modify the details and click "Update"

3. **Deleting a student:**
    - Select a student in the list view
    - Click "Delete" to remove the individual record

4. **Exporting data:**
    - Click "Export to CSV" in the sidebar
    - Data will be exported to `data/students_export.csv`

## 📂 Project Structure

```
StudentDBMS/
├── CMakeLists.txt        # Build configuration
├── main.c                # Application entry point
├── data/                 # Database and logs
│   ├── students.db       # SQLite database
│   ├── activity.log      # Activity logs
│   └── students_export.csv # Exported student data
├── resources/            # UI assets
│   ├── logo.png          # Application logo
│   └── style.css         # Basic CSS
├── src/
│   ├── db/               # Database logic
│   ├── gui/              # GUI components
│   ├── utils/            # Utility functions
│   └── visuals/          # Data visualization
└── README.md             # This file
```

## 🔜 Roadmap

### Coming Soon
- **🔍 Search functionality** - Search and filter student records
- **📊 Enhanced visualizations** - Department labels and additional chart types
- **📱 Responsive design** - Better support for various screen sizes
- **🔄 Data import** - Import student records from CSV files
- **↩️ Undo functionality** - Recover from accidental deletions
- **📄 Pagination** - Better performance with large datasets
- **🖥️ Cross-platform support** - Windows and macOS compatibility

## 🤝 Contributing

Feel free to fork this repository and submit pull requests with improvements!

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 👥 Authors

- **Your Name** - [Vijay Kumar Mahto](https://github.com/vijay_kumar-mahto)

## 📞 Contact

For questions, suggestions, or issues:
- **Email:** [vijaykumar961403@gmail.com](mailto:vijaykumar961403@gmail.com)
- **LinkedIn:** [Vijay Kumar Mahto](https://linkedin.com/in/vijay-kumar-mahto-872901298/)
- **GitHub**: [vijay-kumar-mahto](https://github.com/vijay-kumar-mahto)
- **Issues**: [Report a bug](https://github.com/vijay-kumar-mahto/brick-breaker/issues)


## 🙏 Acknowledgments

- GTK team for the excellent GUI framework
- SQLite team for the reliable embedded database
- All contributors who have helped shape this project

---

<div align="center">
  Made with ❤️ for educational institutions
  <br>
  © 2025 StudentDBMS
</div>