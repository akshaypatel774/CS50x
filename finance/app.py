import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

DEFAULT_CASH = 10000

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

db.execute("""CREATE TABLE IF NOT EXISTS stocks (
    id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    user_id INT,
    symbol VARCHAR(255),
    shares INT,
    price DECIMAL(10, 2),
    operation VARCHAR(10),
    total_share_value DECIMAL(10, 2),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)""")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    stocks = db.execute(
        "SELECT symbol, SUM(CASE WHEN operation = 'buy' THEN shares ELSE -shares END) as shares, price, SUM(CASE WHEN operation = 'buy' THEN total_share_value ELSE -total_share_value END) as total_share_value FROM stocks WHERE user_id = ? GROUP BY symbol",
        session["user_id"],
    )

    cash = db.execute(
        "SELECT cash FROM users WHERE id = ?", session["user_id"])[0]["cash"]

    purchase = db.execute(
        """SELECT SUM(total_share_value) AS total FROM stocks WHERE operation = "buy" AND user_id = ? GROUP BY operation;""", session["user_id"])
    sale = db.execute(
        """SELECT SUM(total_share_value) AS total FROM stocks WHERE operation = "sell" AND user_id = ? GROUP BY operation;""", session["user_id"])

    if purchase and sale:
        total_cash = cash + purchase[0]["total"] - sale[0]["total"]
    else:
        total_cash = cash
    return render_template("index.html", rows=stocks, cash=cash, total=total_cash)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    if request.method == "POST":
        symbol = request.form.get("symbol")
        shares = request.form.get("shares")
        cash = db.execute(
            "SELECT cash FROM users WHERE id = ?", session["user_id"])[0]["cash"]

        if not symbol:
            return apology("Missing symbol", 400)

        try:
            number_of_shares = int(shares)
            if number_of_shares < 1:
                return apology("Invalid share value", 400)
        except ValueError:
            return apology("Invalid share value", 400)

        quote = lookup(symbol)
        if quote == None:
            return apology("Invalid symbol", 400)
        elif not shares:
            return apology("Missing shares", 400)

        shares_value = quote["price"] * number_of_shares
        if shares_value > cash:
            return apology("Insufficient balance", 400)

        db.execute(
            "UPDATE users SET cash = cash - ? WHERE id = ?",
            shares_value,
            session["user_id"],
        )

        db.execute(
            "INSERT INTO stocks (user_id, symbol, shares, price, operation, total_share_value) VALUES (?, ?, ?, ?, ?, ?)",
            session["user_id"],
            symbol.upper(),
            number_of_shares,
            quote["price"],
            "buy",
            shares_value
        )

        flash("Transaction successful")
        return redirect("/")

    return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    stocks = db.execute(
        "SELECT *, CASE WHEN operation = 'sell' THEN -shares ELSE shares END AS shares FROM stocks WHERE user_id = ?",
        session["user_id"],
    )

    return render_template("history.html", rows=stocks)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute(
            "SELECT * FROM users WHERE username = ?", request.form.get("username")
        )

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(
            rows[0]["hash"], request.form.get("password")
        ):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    if request.method == "POST":
        symbol = request.form.get("symbol")
        quote = lookup(symbol)
        if quote == None:
            return apology("Invalid symbol", 400)
        return render_template("quoted.html", price=quote["price"], symbol=quote["symbol"])
    return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure username was submitted
        username = request.form.get("username")
        password = request.form.get("password")
        confirmation = request.form.get("confirmation")

        if not username:
            return apology("must provide username", 400)

        # Ensure password was submitted
        elif not password:
            return apology("must provide password", 400)

        elif not confirmation:
            return apology("must provide confirmation", 400)

        elif confirmation != password:
            return apology("mismatched password", 400)

        elif db.execute("SELECT * FROM users WHERE username = ?", username):
            return apology("Username already taken", 400)

        # Query database for username
        hash = generate_password_hash(password)

        result = db.execute(
            "INSERT INTO users (username, hash) VALUES (?, ?)", username, hash
        )

        session["user_id"] = result

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    if request.method == "POST":
        symbol = request.form.get("symbol")
        shares = request.form.get("shares")

        if not symbol:
            return apology("Missing symbol", 400)

        if not shares:
            return apology("Missing shares", 400)

        quote = lookup(symbol)
        if quote == None:
            return apology("Invalid symbol", 400)

        try:
            number_of_shares = int(shares)
            holdings = db.execute(
                "SELECT SUM(CASE WHEN operation = 'buy' THEN shares ELSE -shares END) as shares FROM stocks WHERE user_id = ? AND symbol = ? GROUP BY symbol",
                session["user_id"],
                symbol
            )[0]["shares"]

            if number_of_shares < 1 or number_of_shares > holdings:
                return apology("Invalid share value", 400)
        except ValueError:
            return apology("Invalid share value", 400)

        shares_value = quote["price"] * number_of_shares

        db.execute(
            "UPDATE users SET cash = cash + ? WHERE id = ?",
            shares_value,
            session["user_id"],
        )

        db.execute(
            "INSERT INTO stocks (user_id, symbol, shares, price, operation, total_share_value) VALUES (?, ?, ?, ?, ?, ?)",
            session["user_id"],
            symbol.upper(),
            number_of_shares,
            quote["price"],
            "sell",
            shares_value
        )

        flash("Transaction successful")

        return redirect("/")

    rows = db.execute("SELECT symbol FROM stocks WHERE user_id = ? GROUP BY symbol",
                      session["user_id"])

    return render_template("sell.html", rows=rows)
