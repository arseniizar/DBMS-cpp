{
  echo "--- FILE: main.cpp ---"
  cat main.cpp

  find engine tests -type f \( -name "*.cpp" -o -name "*.hpp" \) -print | while read -r filename; do
    echo ""
    echo "--- FILE: $filename ---"
    cat "$filename"
  done
} > my_project_code.txt