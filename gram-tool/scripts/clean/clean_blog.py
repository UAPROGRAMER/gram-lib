import re

def clean_blog(data: str) -> str:
    data = re.sub(r"<date>.*</date>", "", data,)
    data = re.sub(r"<.*>", "", data)
    data = re.sub(r"&.*;", "", data)
    data = re.sub(r"urlLink", "", data)
    data = re.sub(r"(http|ftp|https):\/\/.*\s", " ", data)
    data = re.sub(r"\t", " ", data)
    data = re.sub(r"[ ]+", " ", data)
    data = re.sub(r"\n ", "\n", data)
    data = re.sub(r"\n+", "\n", data)
    data = re.sub(r"`|‘|’|‚", "'", data)
    data = re.sub(r"“|”|»|«", "\"", data)
    data = re.sub(r"–|—", "-", data)
    data = re.sub(r" \.", ".", data)
    data = re.sub(r" ,", ",", data)
    data = re.sub(r" :", ":", data)
    data = re.sub(r" ;", ";", data)
    data = re.sub(r".{4,}", "...", data)
    data = re.sub(r"!{4,}", "!!!", data)
    data = data.strip()
    return data