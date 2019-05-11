import os
import pandas as pd

df_EHR = pd.read_csv('EHR-20070101-20170601-nyt-articles-no-duplicates-clean-bodies.csv')
df_PHR = pd.read_csv('PHR-20070101-20170601-nyt-articles-no-duplicates-clean-bodies.csv', encoding='cp1252')

for _, row in df_EHR.iterrows():
    if row['article_body'] and isinstance(row['article_body'], str):
        date = row['date']
        date = date.split('T')[0]
        url = row['url']
        page_name = url.rsplit('/', 1)[1]
        page_name = page_name.split('.', 1)[0]
        article_name = date + '_' + page_name + '.txt'
        filename = os.path.join('articles/EHR', article_name)
        with open(filename, 'w', encoding='utf-8') as article_f:
            article_f.write(row['article_body'])


for _, row in df_PHR.iterrows():
    if row['article_body'] and isinstance(row['article_body'], str):
        date = row['date']
        date = date.split('T')[0]
        url = row['url']
        page_name = url.rsplit('/', 1)[1]
        page_name = page_name.split('.', 1)[0]
        article_name = date + '_' + page_name + '.txt'
        filename = os.path.join('articles/PHR', article_name)
        with open(filename, 'w', encoding='utf-8') as article_f:
            article_f.write(row['article_body'])