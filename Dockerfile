 # Base image with RIOT build and runtime dependencies
 FROM riot/riotbuild:latest

 # Set the working directory
 WORKDIR /riot-app

 # Copy the application code
 COPY . .

 # Build the RIOT application for the native board
 RUN BOARD=native make clean all

 # Default command to run the binary
 CMD ["./src/bin/native/project-digitalization.elf"]
