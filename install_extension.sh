if [ $# -eq 0 ]
  then
    echo "Please provide the path to the SDK as an argument."
    exit 1
fi
# Remove the extension if it already exists
echo "Removing the extension if it already exists..."
rm -rf $1/extension/machine_learning_applications
# Copy the extension to the extension directory of the provided SDK
echo "Copying the extension to the extension directory of the provided SDK..."
cp -r ../machine_learning_applications $1/extension/machine_learning_applications
# Trust the extension using SLC
echo "Trusting the extension using SLC..."
slc signature trust -extpath $1/extension/machine_learning_applications --sdk $1