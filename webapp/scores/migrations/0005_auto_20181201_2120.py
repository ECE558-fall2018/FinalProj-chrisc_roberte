# Generated by Django 2.1.3 on 2018-12-01 21:20

from django.conf import settings
from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('scores', '0004_session_created'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='session',
            name='created',
        ),
        migrations.RemoveField(
            model_name='session',
            name='score',
        ),
        migrations.AlterField(
            model_name='score',
            name='user',
            field=models.OneToOneField(on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL),
        ),
    ]
