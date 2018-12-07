from django.shortcuts import render
from rest_framework.decorators import api_view
from rest_framework.response import Response
from scores.models import Session, Score, UserPress

@api_view(['PUT', 'DELETE'])
def session(request):
    code = request.data.get('id', None)
    if code is None:
        return Response(data={'message': 'JSON must contain "id" field'}, status=400)
    
    code = str(code)
    data = {'request': request.data}
    if request.method == 'PUT':
        obj, created = Session.objects.get_or_create(code=code)
        if created:
            status = 200
            message = 'Session has been created'
        else:
            status = 202
            message = 'Session already existed'
    elif request.method == 'DELETE':
        objs = Session.objects.filter(code=code)
        if not objs:
            status = 404
            message = 'Can not delete session. Resource not found'
            
        else:
            objs.delete()
            status = 200
            message = 'Session deleted'
            data['id'] = code

    data['message'] = message
    data['status'] = status
        
    return Response(data=data, status=status)

@api_view(['GET', 'POST', 'DELETE'])
def session_with_urlkwarg(request, code):
    session = None
    if request.method == 'GET':
        try:
            session = Session.objects.get(code=code)
            message = 'Session found'
            status = 200
        except Session.DoesNotExist:
            message = 'Session not found with code: ' + code
            status = 404
            pass
    
    if request.method == 'POST':
        session, created = Session.objects.get_or_create(code=code)
        message = 'Session already exists found' if not created else 'Session created'
        status = 200

    if request.method == 'DELETE' or 'delete' in request.query_params:
        objs = Session.objects.filter(code=code)
        if not objs:
            status = 404
            message = 'Can not delete session. Resource not found'
        else:
            objs.delete()
            status = 200
            message = 'Session deleted'

    data = dict()
    data['session_code'] = code
    data['message'] = message
    data['status'] = status
    if session and session.user:
        username = session.user.username
    else:
        username = None

    data['user'] = username

    return Response(data=str(username), status=status, content_type='text/plain')

@api_view(['GET', 'POST'])
def score(request, username):
    obj, created = Score.objects.get_or_create(user__username=username)
    if created:
        obj.score = 0
        obj.save()

    if request.method == 'POST':
        newscore = request.query_params.get('score', None)
        if not newscore:
            return Response(data={'score': 'field missing, please include a score field'}, status=400)

        obj.score = max(int(newscore), int(obj.score))
        obj.save()

    return Response(data=obj.score, status=200, content_type='text/plain')

@api_view(['GET'])
def user(request, code):
    user = None
    try:
        user = Session.objects.get(code=code).user
    except Session.DoesNotExist:
        pass

    if not user:
        return Response(data={'message': 'no user exists for session: ' + code}, status=404)
    
    return Response(data={'username': user.username }, status=200)


@api_view(['GET', 'POST'])
def userpress(request, username):
    code = ""
    try:
        press = UserPress.objects.get(user__username=username)
        code = press.code
    except UserPress.DoesNotExist:
        pass

    code = code.strip()

    print("return code:", code)

    return Response(data=code, status=200, content_type='text/plain')
