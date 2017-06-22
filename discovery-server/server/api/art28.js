import { Router } from 'express'

var router = Router()

var nodes = [
  {ssid: 'test', ip: '255.255.255.255', mac: 'fe:ed:de:ad:be:ef'}
];

router.get('/nodes', (req, res, next) => {
  res.json(nodes);
})

router.post('/nodes/new', (req, res, next) => {
  let node = req.body;
  console.log(req.body);
  if (nodes.length > 20) nodes.pop();
  nodes.unshift(node);
  res.sendStatus(201);
})

export default router
