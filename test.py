from flask import Flask, request
books = []
app = Flask(__name__)
@app.route('/book', methods=['POST', 'GET'])
def book():
  if request.method == 'POST':
    body = request.get_json()
    books.append(body)
    
    return { "message": "Book already add to database", "body": body }, 201
  elif request.method == 'GET':
    return { "books": books }, 200
