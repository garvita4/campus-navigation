cat << 'EOF' > Dockerfile
FROM gcc:latest
WORKDIR /app
COPY . .
RUN make
EXPOSE 8080
CMD ["./campus", "--serve"]
EOF